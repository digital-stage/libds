#include "DigitalStage/Api/Client.h"
#include "DigitalStage/Api/Events.h"

#include <exception>
#include <iostream>
#include <memory>
#include <utility>

#include "spdlog/spdlog.h"

namespace DigitalStage::Api
{

    std::string describe_broken_json(nlohmann::json const& broken)
    {
        if (broken.is_null()) {
            return "<nulljson>!";
        } else if (broken.is_array()) {
            if (broken.size() > 0) {
                return fmt::format("Array [{}...]", describe_broken_json(broken[0]));
            } else {
                return "empty array";
            }
        } else {
            return broken.dump();
        }
    }

    Client::Client(std::string const & apiUrl) : apiUrl_(apiUrl)
    {
        store_ = std::make_unique<Store>();
        wsclient_ = std::make_unique<teckos::client>();
        wsclient_->setShouldReconnect(true);
        wsclient_->setSendPayloadOnReconnect(true);
        wsclient_->setReconnectTrySleep(std::chrono::milliseconds(500)); // 500ms between tries for retry
    }

    Client::~Client()
    {
        disconnect();
    }

    std::weak_ptr<Store> Client::getStore() const
    {
        return this->store_;
    }

    void Client::disconnect()
    {
        wsclient_->on_disconnected({});
        wsclient_->on_reconnected({});
        wsclient_->setMessageHandler({});
        wsclient_->disconnect();
    }

    bool Client::isConnected()
    {
        if (wsclient_) {
            return wsclient_->isConnected();
        }
        return false;
    }

    void Client::connect(const std::string & apiToken, const nlohmann::json & initialDevice)
    {
        // Set handler
        wsclient_->on_disconnected([this](bool expected) { disconnected(expected); });
        wsclient_->on_reconnected([]() { spdlog::info("Libds reconnected"); });
        wsclient_->setMessageHandler([this](const nlohmann::json & json) {
            try {
                if (!json.is_array()) {
                    // This is actually guaranteed by libteckos, so this will never happen
                    assert(false);
                    throw InvalidPayloadException("Response from server is invalid");
                }
                const std::string & event = json[0];
                const nlohmann::json payload = (json.size() > 1) ? json[1] : nlohmann::json::object();
                handleMessage(event, payload);
            } catch (const std::exception & e) {
                spdlog::error("Libds caught exception in message handler handling {}, sending error signal: {}", json.dump(), e.what());
                error(e);
            } catch (...) {
                spdlog::error("Libds caught unexpected exception in message handler handling {}, no further error handling possible.", json.dump());
            }
        });

        // Handlers set, now connect the websocket
        wsclient_->connect(apiUrl_, apiToken, {{"device", initialDevice}});
    }

    void Client::send(const std::string & event, const nlohmann::json & message)
    {
#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
        spdlog::debug("[SENDING] {}: {}", event, message);
#else
        spdlog::debug("[SENDING] {}", event);
#endif
#endif
        if (!wsclient_) {
            throw std::runtime_error("Libds not ready");
        }
        wsclient_->send(event, message, [event, message](teckos::Result result) {
            spdlog::debug("Got result from client, ignoring it: {}", result.size() > 0 ? result[0].dump() : "empty array");
        });
    }

    void Client::send(const std::string & event, const nlohmann::json & message, teckos::Callback callback)
    {
#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
        spdlog::debug("[SENDING] {}: {}", event, message);
#else
        spdlog::debug("[SENDING] {}", event);
#endif
#endif
        if (!wsclient_) {
            throw std::runtime_error("Libds not ready");
        }
        wsclient_->send(event, message, callback);
    }

    std::future<std::pair<std::string, std::optional<std::string>>> Client::decodeInvitationCode(const std::string & code)
    {
        using InvitePromise = std::promise<std::pair<std::string, std::optional<std::string>>>;
        auto const promise = std::make_shared<InvitePromise>();
        try {
            wsclient_->send("decode-invite", code, [promise](const std::vector<nlohmann::json> & result) {
                try {
                    if (result.size() > 1 && !result[1].is_null()) {
                        if (result[1].count("groupId") != 0 && !result[1]["groupId"].is_null()) {
                            std::string groupId = result[1]["groupId"];
                            promise->set_value({result[1]["stageId"], groupId});
                        } else {
                            promise->set_value({result[1]["stageId"], std::nullopt});
                        }
                    } else if (result.size() == 1) {
                        promise->set_exception(std::make_exception_ptr(std::runtime_error(result[0].dump())));
                    } else {
                        promise->set_exception(std::make_exception_ptr(std::runtime_error("Unexpected communication error")));
                    }
                } catch (std::exception & e) {
                    // JSON access errors for example might end up here
                    promise->set_exception(std::make_exception_ptr(e));
                }
            });
        } catch (std::exception & e) {
            spdlog::error("Libds caught unexpected exception during decode invitation code: {}", e.what());
            promise->set_exception(std::make_exception_ptr(e));
        }
        return promise->get_future();
    }

    std::future<std::string> Client::revokeInvitationCode(const std::string & stageId, const std::optional<std::string> & groupId)
    {
        nlohmann::json payload{};
        payload["stageId"] = stageId;
        if (groupId) {
            payload["groupId"] = *groupId;
        }
        using InvitePromise = std::promise<std::string>;
        auto const promise = std::make_shared<InvitePromise>();
        try {
            wsclient_->send("revoke-invite", payload, [promise](const std::vector<nlohmann::json> & result) {
                if (result.size() > 1) {
                    promise->set_value(result[1]);
                } else if (result.size() == 1) {
                    promise->set_exception(std::make_exception_ptr(std::runtime_error(describe_broken_json(result[0]))));
                } else {
                    promise->set_exception(std::make_exception_ptr(std::runtime_error("Unexpected communication error")));
                }
            });
        } catch (std::exception & e) {
            spdlog::error("Libds caught unexpected exception during revoke invitation code: {}", e.what());
            promise->set_exception(std::make_exception_ptr(e));
        }
        return promise->get_future();
    }

    std::future<std::string> Client::encodeInvitationCode(const std::string & stageId, const std::optional<std::string> & groupId)
    {
        nlohmann::json payload{};
        payload["stageId"] = stageId;
        if (groupId) {
            payload["groupId"] = *groupId;
        }
        using InvitePromise = std::promise<std::string>;
        auto const promise = std::make_shared<InvitePromise>();
        try {
            wsclient_->send("encode-invite", payload, [promise](const std::vector<nlohmann::json> & result) {
                if (result.size() > 1) {
                    promise->set_value(result[1]);
                } else if (result.size() == 1) {
                    promise->set_exception(std::make_exception_ptr(std::runtime_error(describe_broken_json(result[0]))));
                } else {
                    promise->set_exception(std::make_exception_ptr(std::runtime_error("Unexpected communication error")));
                }
            });
        } catch (std::exception & e) {
            spdlog::error("Libds caught unexpected exception during encode invitation code: {}", e.what());
            promise->set_exception(std::make_exception_ptr(e));
        }
        return promise->get_future();
    }

    std::future<bool> Client::joinStage(std::string stageId, std::optional<std::string> const & groupId, std::optional<std::string> const & password)
    {
        nlohmann::json payload{};
        payload["stageId"] = std::move(stageId);
        if (groupId) {
            payload["groupId"] = *groupId;
        }
        if (password) {
            payload["password"] = *password;
        }

        using JoinPromise = std::promise<bool>;
        auto const promise = std::make_shared<JoinPromise>();
        try {
            wsclient_->send("join-stage", payload, [promise](const std::vector<nlohmann::json> & result) {
                if (result.size() == 1 && !result[0].is_null()) {
                    if (result[0].is_string() && result[0] == "Invalid password") {
                        promise->set_value(false);
                    } else {
                        promise->set_exception(std::make_exception_ptr(std::runtime_error("Unknown API response.")));
                    }
                } else {
                    promise->set_value(true);
                }
            });
        } catch (std::exception & e) {
            spdlog::error("Libds caught unexpected exception during joinstage: {}", e.what());
            promise->set_exception(std::make_exception_ptr(e));
        }
        return promise->get_future();
    }

    template <typename ValueTypeCV, typename ValueType = nlohmann::detail::uncvref_t<ValueTypeCV>> ValueTypeCV parse(const nlohmann::json& json, const std::string& event, const std::string& className)
    {
        try {
            return json.get<ValueTypeCV>();
        }
        catch (const DigitalStage::Types::ParseException& e) {
            throw DigitalStage::Api::InvalidPayloadException("Error parsing " + className + " when handling event '" + event + "': " + e.what());
        }
        catch (const std::exception& e) {
            throw DigitalStage::Api::InvalidPayloadException("Unknown error while parsing " + className + " of event '" + event + "': " + e.what());
        }
    }

    template <typename ValueTypeCV, typename ValueType = nlohmann::detail::uncvref_t<ValueTypeCV>> ValueTypeCV parseKey(const nlohmann::json& payload, const std::string& key, const std::string& event)
    {
        if (!payload.contains(key)) {
            throw InvalidPayloadException("Payload for 's-j' event is missing the key 'stageID'");
        }
        try {
            return payload[key].get<ValueTypeCV>();
        }
        catch (const DigitalStage::Types::ParseException& e) {
            throw DigitalStage::Api::InvalidPayloadException("Error parsing key " + key + " of payload from event '" + event + "': " + e.what());
        }
        catch (const std::exception& e) {
            throw DigitalStage::Api::InvalidPayloadException("Unknown error while parsing key" + key + " of event '" + event + "': " + e.what());
        }
    }

    void Client::handleMessage(const std::string& event, const nlohmann::json& payload)
    {
#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
        spdlog::debug("[EVENT] {}: {}", event, payload.dump());
#else
        spdlog::debug("[EVENT] {}", event);
#endif
#endif
        if (event == RetrieveEvents::READY) {
            store_->setReady(true);
            if (payload.contains("turn")) {
                // TODO Is this optional or is it an error when these fields are missing? Missing verbosity!
                store_->setTurnServers(payload["turn"]["urls"]);
                store_->setTurnUsername(payload["turn"]["username"]);
                store_->setTurnPassword(payload["turn"]["credential"]);
            }
            ready(getStore());
        }
        else if (event == RetrieveEvents::TURN_SERVERS_CHANGED) {
            store_->setTurnServers(payload);
            // TODO no signal sent
        }
        /*
         * LOCAL DEVICE
         */
        else if (event == RetrieveEvents::LOCAL_DEVICE_READY) {
            const auto device = parse<Device>(payload, event, "Device");
            store_->devices.create(payload);
            store_->setLocalDeviceId(device._id);

            deviceAdded(device, getStore());
            localDeviceReady(device, getStore());
            audioDriverSelected(device.audioDriver, getStore());
            inputSoundCardSelected(device.inputSoundCardId, getStore());
            outputSoundCardSelected(device.outputSoundCardId, getStore());
        }
        /*
         * LOCAL USER
         */
        else if (event == RetrieveEvents::USER_READY) {
            const auto user = parse<User>(payload, event, "User");
            store_->users.create(payload);
            store_->setUserId(user._id);

            userAdded(user, getStore());
            localUserReady(user, getStore());
        }
        /*
         * DEVICES
         */
        else if (event == RetrieveEvents::DEVICE_ADDED) {
            const auto device = parse<Device>(payload, event, "Device");
            store_->devices.create(payload);

            deviceAdded(device, getStore());
        }
        else if (event == RetrieveEvents::DEVICE_CHANGED) {
            store_->devices.update(payload);
            const auto id = parseKey<ID_TYPE>(payload, "_id", event);
            deviceChanged(id, payload, getStore());
            auto localDeviceIdPtr = store_->getLocalDeviceId();
            if (localDeviceIdPtr && *localDeviceIdPtr == id) {
                auto device = store_->devices.get(id);
                localDeviceChanged(id, payload, getStore());
                if (payload.count("audioDriver") != 0) {
                    audioDriverSelected(device->audioDriver, getStore());
                }
                if (payload.count("inputSoundCardId") != 0) {
                    inputSoundCardSelected(device->inputSoundCardId, getStore());
                }
                if (payload.count("outputSoundCardId") != 0) {
                    outputSoundCardSelected(device->outputSoundCardId, getStore());
                }
            }
        }
        else if (event == RetrieveEvents::DEVICE_REMOVED) {
            const auto id = parse<ID_TYPE>(payload, event, "id");
            store_->devices.remove(id);
            deviceRemoved(id, getStore());
        }
        /*
         * STAGE
         */
        else if (event == RetrieveEvents::STAGE_ADDED) {
            const auto stage = parse<Stage>(payload, event, "Stage");
            store_->stages.create(payload);

            stageAdded(stage, getStore());
        }
        else if (event == RetrieveEvents::STAGE_CHANGED) {
            store_->stages.update(payload);
            const auto id = parseKey<ID_TYPE>(payload, "_id", event);

            stageChanged(id, payload, getStore());
        }
        else if (event == RetrieveEvents::STAGE_REMOVED) {
            const auto id = parse<ID_TYPE>(payload, event, "id");
            store_->stages.remove(id);

            stageRemoved(id, getStore());
        }
        /*
         * GROUPS
         */
        else if (event == RetrieveEvents::GROUP_ADDED) {
            const auto group = parse<Group>(payload, event, "Group");
            store_->groups.create(payload);

            groupAdded(group, getStore());
        }
        else if (event == RetrieveEvents::GROUP_CHANGED) {
            store_->groups.update(payload);
            const auto id = parseKey<ID_TYPE>(payload, "_id", event);

            groupChanged(id, payload, getStore());
        }
        else if (event == RetrieveEvents::GROUP_REMOVED) {
            const auto id = parse<ID_TYPE>(payload, event, "id");
            store_->groups.remove(id);

            groupRemoved(id, getStore());
        }
        /*
         * CUSTOM GROUP
         */
        else if (event == RetrieveEvents::CUSTOM_GROUP_ADDED) {
            const auto customGroup = parse<CustomGroup>(payload, event, "CustomGroup");
            store_->customGroups.create(payload);

            customGroupAdded(customGroup, getStore());
        }
        else if (event == RetrieveEvents::CUSTOM_GROUP_CHANGED) {
            store_->customGroups.update(payload);
            const auto id = parseKey<ID_TYPE>(payload, "_id", event);

            customGroupChanged(id, payload, getStore());
        }
        else if (event == RetrieveEvents::CUSTOM_GROUP_REMOVED) {
            const auto id = parse<ID_TYPE>(payload, event, "id");
            auto custom_group = store_->customGroups.get(id);
            if (custom_group) {
                store_->customGroups.remove(id);
                customGroupRemoved(*custom_group, getStore());
            }
        }
        /*
         * STAGE MEMBERS
         */
        else if (event == RetrieveEvents::STAGE_MEMBER_ADDED) {
            const auto stage_member = parse<StageMember>(payload, event, "StageMember");
            store_->stageMembers.create(payload);

            stageMemberAdded(stage_member, getStore());
        }
        else if (event == RetrieveEvents::STAGE_MEMBER_CHANGED) {
            store_->stageMembers.update(payload);
            const auto id = parseKey<ID_TYPE>(payload, "_id", event);

            stageMemberChanged(id, payload, getStore());
            if (id == store_->getStageMemberId()) {
                if (payload.count("groupId") != 0) {
                    if (payload["groupId"].is_null()) {
                        store_->setGroupId(std::nullopt);
                    }
                    else {
                        store_->setGroupId(payload["groupId"].get<ID_TYPE>());
                    }
                }
            }
        }
        else if (event == RetrieveEvents::STAGE_MEMBER_REMOVED) {
            const auto id = parse<ID_TYPE>(payload, event, "id");
            store_->stageMembers.remove(id);

            stageMemberRemoved(id, getStore());
        }
        /*
         * STAGE DEVICES
         */
        else if (event == RetrieveEvents::STAGE_DEVICE_ADDED) {
            const auto stageDevice = parse<StageDevice>(payload, event, "StageDevice");
            store_->stageDevices.create(payload);
            auto localDeviceId = store_->getLocalDeviceId();
            auto stageId = store_->getStageId();
            if (localDeviceId && stageId && *stageId == stageDevice.stageId && *localDeviceId == stageDevice.deviceId) {
                store_->setStageDeviceId(stageDevice._id);
            }

            stageDeviceAdded(stageDevice, getStore());
        }
        else if (event == RetrieveEvents::STAGE_DEVICE_CHANGED) {
            store_->stageDevices.update(payload);
            const auto id = parseKey<ID_TYPE>(payload, "_id", event);

            stageDeviceChanged(id, payload, getStore());
        }
        else if (event == RetrieveEvents::STAGE_DEVICE_REMOVED) {
            const auto id = parse<ID_TYPE>(payload, event, "id");
            auto stageDevice = store_->stageDevices.get(id);
            if (stageDevice) {
                store_->stageDevices.remove(id);
                auto stageId = store_->getStageId();
                auto localDeviceId = store_->getLocalDeviceId();
                if (localDeviceId && stageId && *stageId == stageDevice->stageId && *localDeviceId == stageDevice->deviceId) {
                    store_->resetStageDeviceId();
                }

                stageDeviceRemoved(*stageDevice, getStore());
            }
        }
        /*
         * VIDEO TRACKS
         */
        else if (event == RetrieveEvents::VIDEO_TRACK_ADDED) {
            const auto videoTrack = parse<VideoTrack>(payload, event, "VideoTrack");
            store_->videoTracks.create(payload);

            videoTrackAdded(videoTrack, getStore());
        }
        else if (event == RetrieveEvents::VIDEO_TRACK_CHANGED) {
            store_->videoTracks.update(payload);
            const auto id = parseKey<ID_TYPE>(payload, "_id", event);

            videoTrackChanged(id, payload, getStore());
        }
        else if (event == RetrieveEvents::VIDEO_TRACK_REMOVED) {
            const auto id = parse<ID_TYPE>(payload, event, "id");
            auto track = store_->videoTracks.get(id);
            store_->videoTracks.remove(id);

            videoTrackRemoved(*track, getStore());
        }
        /*
         * AUDIO TRACKS
         */
        else if (event == RetrieveEvents::AUDIO_TRACK_ADDED) {
            const auto audioTrack = parse<AudioTrack>(payload, event, "AudioTrack");
            store_->audioTracks.create(payload);

            audioTrackAdded(audioTrack, getStore());
        }
        else if (event == RetrieveEvents::AUDIO_TRACK_CHANGED) {
            store_->audioTracks.update(payload);
            const auto id = parseKey<ID_TYPE>(payload, "_id", event);

            audioTrackChanged(id, payload, getStore());
        }
        else if (event == RetrieveEvents::AUDIO_TRACK_REMOVED) {
            const auto id = parse<ID_TYPE>(payload, event, "id");
            auto track = store_->audioTracks.get(id);
            store_->audioTracks.remove(id);

            audioTrackRemoved(*track, getStore());
        }
        /*
         * USERS
         */
        else if (event == RetrieveEvents::USER_ADDED) {
            const auto user = parse<User>(payload, event, "User");
            store_->users.create(payload);

            userAdded(user, getStore());
        }
        else if (event == RetrieveEvents::USER_CHANGED) {
            store_->users.update(payload);
            const auto id = parseKey<ID_TYPE>(payload, "_id", event);

            userChanged(id, payload, getStore());
        }
        else if (event == RetrieveEvents::USER_REMOVED) {
            const auto id = parse<ID_TYPE>(payload, event, "id");
            store_->users.remove(id);

            userRemoved(id, getStore());
        }
        /*
         * SOUND CARD
         */
        else if (event == RetrieveEvents::SOUND_CARD_ADDED) {
            const auto soundCard = parse<SoundCard>(payload, event, "SoundCard");
            store_->soundCards.create(payload);

            soundCardAdded(soundCard, getStore());
        }
        else if (event == RetrieveEvents::SOUND_CARD_CHANGED) {
            store_->soundCards.update(payload);
            const auto id = parseKey<ID_TYPE>(payload, "_id", event);
            soundCardChanged(id, payload, getStore());
            auto localDevice = store_->getLocalDevice();
            if (localDevice) {
                if (localDevice->inputSoundCardId == id) {
                    inputSoundCardChanged(id, payload, getStore());
                }
                if (localDevice->outputSoundCardId == id) {
                    outputSoundCardChanged(id, payload, getStore());
                }
            }
        }
        else if (event == RetrieveEvents::SOUND_CARD_REMOVED) {
            const auto id = parse<ID_TYPE>(payload, event, "id");
            store_->soundCards.remove(id);

            soundCardRemoved(id, getStore());
        }
        /*
         * STAGE JOINED
         */
        else if (event == RetrieveEvents::STAGE_JOINED) {
            auto stageId = parseKey<std::string>(payload, "stageId", event);
            auto stageMemberId = parseKey<std::string>(payload, "stageMemberId", event);
            if (!payload.contains("groupId")) throw InvalidPayloadException("No groupId in payload of event " + event);
            auto groupId = payload["groupId"].is_null() ? std::nullopt : std::optional<std::string>(parseKey<std::string>(payload, "groupId", event));
            auto localDeviceId = store_->getLocalDeviceId();
            if (payload.count("remoteUsers") > 0) {
                for (const auto& item : payload["remoteUsers"]) {
                    const auto user = parse<User>(item, event, "User");
                    store_->users.create(item);
                    userAdded(user, getStore());
                }
            }
            if (payload.count("stage") > 0) {
                const auto stage = parse<Stage>(payload["stage"], event, "Stage");
                store_->stages.create(payload["stage"]);
                stageAdded(stage, getStore());
            }
            if (payload.count("groups") > 0) {
                for (const auto& item : payload["groups"]) {
                    const auto group = parse<Group>(item, event, "Group");
                    store_->groups.create(item);
                    groupAdded(group, getStore());
                }
            }
            if (payload.contains("customGroups")) {
                for (const auto& item : payload["customGroups"]) {
                    const auto customGroup = parse<CustomGroup>(item, event, "CustomGroup");
                    store_->customGroups.create(item);
                    customGroupAdded(customGroup, getStore());
                }
            }
            for (const auto& item : payload["stageMembers"]) {
                const auto stage_member = parse<StageMember>(item, event, "StageMember");
                store_->stageMembers.create(item);
                stageMemberAdded(stage_member, getStore());
            }
            for (const auto& item : payload["stageDevices"]) {
                const auto stageDevice = parse<StageDevice>(item, event, "StageDevice");
                store_->stageDevices.create(item);
                if (localDeviceId && stageId == stageDevice.stageId && *localDeviceId == stageDevice.deviceId) {
                    store_->setStageDeviceId(stageDevice._id);
                }
                stageDeviceAdded(stageDevice, getStore());
            }
            for (const auto& item : payload["audioTracks"]) {
                const auto audioTrack = parse<AudioTrack>(item, event, "AudioTrack");
                store_->audioTracks.create(item);
                audioTrackAdded(audioTrack, getStore());
            }
            for (const auto& item : payload["videoTracks"]) {
                const auto videoTrack = parse<VideoTrack>(item, event, "VideoTrack");
                store_->videoTracks.create(item);
                videoTrackAdded(videoTrack, getStore());
            }
            store_->setStageId(stageId);
            store_->setGroupId(groupId);
            store_->setStageMemberId(stageMemberId);
            stageJoined(stageId, groupId, getStore());
        }
        /*
         * STAGE LEFT
         */
        else if (event == RetrieveEvents::STAGE_LEFT) {
            store_->resetStageId();
            store_->resetGroupId();
            store_->resetStageMemberId();
            store_->resetStageDeviceId();
            store_->stageMembers.removeAll();
            store_->customGroups.removeAll();
            store_->videoTracks.removeAll();
            store_->audioTracks.removeAll();
            // TODO: Discuss, the store may dispatch all the events instead...
            // TODO: Otherwise we have to dispatch all removals HERE (!)
            // Current workaround: assuming, that on left all using
            // components know, that the entities are removed without event
            stageLeft(getStore());
        }
        // WebRTC
        else if (event == RetrieveEvents::P2P_RESTART) {
            const auto item = parse<P2PRestart>(payload, event, "P2PRestart");
            p2pRestart(item, getStore());
        }
        else if (event == RetrieveEvents::P2P_OFFER_SENT) {
            const auto item = parse<P2POffer>(payload, event, "P2POffer");
            p2pOffer(item, getStore());
        }
        else if (event == RetrieveEvents::P2P_ANSWER_SENT) {
            const auto item = parse<P2PAnswer>(payload, event, "P2PAnswer");
            p2pAnswer(item, getStore());
        }
        else if (event == RetrieveEvents::ICE_CANDIDATE_SENT) {
            const auto item = parse<IceCandidate>(payload, event, "IceCandidate");
            iceCandidate(item, getStore());
        }
    }

} // namespace DigitalStage::Api
