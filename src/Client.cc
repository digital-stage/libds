#include "DigitalStage/Api/Client.h"
#include "DigitalStage/Api/Events.h"

#include <exception>
#include <memory>
#include <utility>
#include <iostream>

using namespace DigitalStage::Api;

Client::Client(std::string apiUrl, bool async_events)
    : apiUrl_(std::move(apiUrl)) {
  store_ = std::make_unique<Store>();
  wsclient_ = std::make_unique<teckos::client>(async_events);
  wsclient_->setReconnect(true);
  wsclient_->sendPayloadOnReconnect(true);
}

Client::~Client() {
  wsclient_->on_disconnected(nullptr);
  wsclient_->setMessageHandler(nullptr);
}

Store *Client::getStore() const {
  return this->store_.get();
}

void Client::disconnect() {
  wsclient_->disconnect();
}

bool Client::isConnected() {
  if (wsclient_)
    return wsclient_->isConnected();
  return false;
}

void Client::connect(const std::string &apiToken,
                     const nlohmann::json &initialDevice) {
  // Set handler
  wsclient_->on_disconnected([this](bool expected) {
    std::cout << "Disconnected" << std::endl;
    disconnected(expected);
  });
  wsclient_->on_reconnected([]() {
    std::cout << "Reconnected" << std::endl;
  });
  wsclient_->setMessageHandler([this](const nlohmann::json &json) {
    try {
      if (!json.is_array()) {
        throw InvalidPayloadException("Response from server is invalid");
      }
      const std::string &event = json[0];
      const nlohmann::json payload = (json.size() > 1) ? json[1] : nlohmann::json::object();
      handleMessage(event, payload);
    } catch (const std::exception &e) {
      //TODO: Maybe we can remove the output to std::err here?
      std::cerr << e.what() << std::endl;
      error(e);
    }
  });

  std::string apiUrl(this->apiUrl_.begin(), this->apiUrl_.end());
  return wsclient_->connect(apiUrl, apiToken,
                            {{"device", initialDevice}});
}

void Client::send(const std::string &event,
                  const nlohmann::json &message) {
#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
  std::cout << "[SENDING] " << event << ": " << message;
#else
  std::cout << "[SENDING] " << event;
#endif
#endif
  if (!wsclient_)
    throw std::runtime_error("Not ready");
  return wsclient_->send(event, message);
}

void Client::send(
    const std::string &event, const nlohmann::json &message,
    teckos::Callback callback) {
#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
  std::cout << "[SENDING] " << event << ": " << message;
#else
  std::cout << "[SENDING] " << event;
#endif
#endif
  if (!wsclient_)
    throw std::runtime_error("Not ready");
  return wsclient_->send(event, message, callback);
}

[[maybe_unused]] DigitalStage::Types::WholeStage Client::getWholeStage() const {
  const std::lock_guard<std::mutex> lock(wholeStage_mutex_);
  return wholeStage_.get<DigitalStage::Types::WholeStage>();
}

[[maybe_unused]] void Client::setWholeStage(nlohmann::json wholeStage) {
  const std::lock_guard<std::mutex> lock(wholeStage_mutex_);
  this->wholeStage_ = std::move(wholeStage);
}

[[maybe_unused]] std::future<std::pair<std::string,
                                       std::string>>
Client::decodeInvitationCode(const std::string &code) {
  using InvitePromise = std::promise<std::pair<std::string, std::string>>;
  auto const promise = std::make_shared<InvitePromise>();
  wsclient_->send("decode-invite", code, [promise](const std::vector<nlohmann::json> &result) {
    if (result.size() > 1 && !result[1].is_null()) {
      if (result[1].count("groupId") != 0 && !result[1]["groupId"].is_null()) {
        promise->set_value({result[1]["stageId"], result[1]["groupId"]});
      } else {
        promise->set_value({result[1]["stageId"], {}}); //TODO: This throws (!)
      }
    } else if (result.size() == 1) {
      promise->set_exception(std::make_exception_ptr(std::runtime_error(result[0])));
    } else {
      promise->set_exception(std::make_exception_ptr(std::runtime_error("Unexpected communication error")));
    }
  });
  return promise->get_future();
}

[[maybe_unused]] std::future<std::string> Client::revokeInvitationCode(const std::string &stageId,
                                                                       const std::string &groupId) {
  nlohmann::json payload;
  payload["stageId"] = stageId;
  payload["groupId"] = groupId;
  using InvitePromise = std::promise<std::string>;
  auto const promise = std::make_shared<InvitePromise>();
  wsclient_->send("revoke-invite", payload, [promise](const std::vector<nlohmann::json> &result) {
    if (result.size() > 1) {
      promise->set_value(result[1]);
    } else if (result.size() == 1) {
      promise->set_exception(std::make_exception_ptr(std::runtime_error(result[0])));
    } else {
      promise->set_exception(std::make_exception_ptr(std::runtime_error("Unexpected communication error")));
    }
  });
  return promise->get_future();
}

[[maybe_unused]] std::future<std::string> Client::encodeInvitationCode(const std::string &stageId,
                                                                       const std::string &groupId) {
  nlohmann::json payload;
  payload["stageId"] = stageId;
  payload["groupId"] = groupId;
  using InvitePromise = std::promise<std::string>;
  auto const promise = std::make_shared<InvitePromise>();
  wsclient_->send("encode-invite", payload, [promise](const std::vector<nlohmann::json> &result) {
    if (result.size() > 1) {
      promise->set_value(result[1]);
    } else if (result.size() == 1) {
      promise->set_exception(std::make_exception_ptr(std::runtime_error(result[0])));
    } else {
      promise->set_exception(std::make_exception_ptr(std::runtime_error("Unexpected communication error")));
    }
  });
  return promise->get_future();
}

template<typename ValueTypeCV, typename ValueType = nlohmann::detail::uncvref_t<ValueTypeCV>>
ValueTypeCV parse(const nlohmann::json &json, const std::string &event, const std::string &name) {
  try {
    return json.get<ValueTypeCV>();
  } catch (const DigitalStage::Types::ParseException &e) {
    throw DigitalStage::Api::InvalidPayloadException("Error parsing " + name + " when handling event '" + event + "': " + e.what());
  } catch (const nlohmann::json::exception &e) {
    throw DigitalStage::Api::InvalidPayloadException("Unhandled json error when parsing " + name + " of event '" + event + "': " + e.what());
  }
}

void Client::handleMessage(const std::string &event, const nlohmann::json &payload) {
#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
  std::cout << "[EVENT] " << event << " " << payload.dump() << std::endl;
#else
  std::cout << "[EVENT] " << event << std::endl;
#endif
#endif

  if (event == RetrieveEvents::READY) {
    store_->setReady(true);
    if (payload.contains("turn")) {
      store_->setTurnServers(payload["turn"]["urls"]);
      store_->setTurnUsername(payload["turn"]["username"]);
      store_->setTurnPassword(payload["turn"]["credential"]);
    }
    this->ready(getStore());

  } else if (event == RetrieveEvents::TURN_SERVERS_CHANGED) {
    store_->setTurnServers(payload);

    /*
     * LOCAL DEVICE
     */
  } else if (event == RetrieveEvents::LOCAL_DEVICE_READY) {
    const auto device = parse<Device>(payload, event, "Device");
    store_->devices.create(payload);
    store_->setLocalDeviceId(device._id);
    this->deviceAdded(device, getStore());
    this->localDeviceReady(device, getStore());
    this->audioDriverSelected(device.audioDriver, getStore());
    this->inputSoundCardSelected(device.inputSoundCardId, getStore());
    this->outputSoundCardSelected(device.outputSoundCardId, getStore());
    /*
     * LOCAL USER
     */
  } else if (event == RetrieveEvents::USER_READY) {
    const auto user = parse<User>(payload, event, "User");
    store_->users.create(payload);
    store_->setUserId(user._id);
    this->userAdded(user, getStore());
    this->localUserReady(user, getStore());

    /*
     * DEVICES
     */
  } else if (event == RetrieveEvents::DEVICE_ADDED) {
    const auto device = parse<Device>(payload, event, "Device");
    store_->devices.create(payload);
    this->deviceAdded(device, getStore());
  } else if (event == RetrieveEvents::DEVICE_CHANGED) {
    store_->devices.update(payload);
    const ID_TYPE id = payload["_id"];
    this->deviceChanged(id, payload, getStore());
    auto localDeviceIdPtr = store_->getLocalDeviceId();
    if (localDeviceIdPtr && *localDeviceIdPtr == id) {
      auto device = store_->devices.get(id);
      this->localDeviceChanged(id, payload, getStore());
      if (payload.count("audioDriver") != 0) {
        this->audioDriverSelected(device->audioDriver, getStore());
      }
      if (payload.count("inputSoundCardId") != 0) {
        this->inputSoundCardSelected(device->inputSoundCardId, getStore());
      }
      if (payload.count("outputSoundCardId") != 0) {
        this->outputSoundCardSelected(device->outputSoundCardId, getStore());
      }
    }
  } else if (event == RetrieveEvents::DEVICE_REMOVED) {
    const ID_TYPE id = payload;
    store_->devices.remove(id);
    this->deviceRemoved(id, getStore());

    /*
     * STAGE
     */
  } else if (event == RetrieveEvents::STAGE_ADDED) {
    const auto stage = parse<Stage>(payload, event, "Stage");
    store_->stages.create(payload);
    this->stageAdded(stage, getStore());
  } else if (event == RetrieveEvents::STAGE_CHANGED) {
    store_->stages.update(payload);
    const ID_TYPE id = payload["_id"];
    this->stageChanged(id, payload, getStore());
  } else if (event == RetrieveEvents::STAGE_REMOVED) {
    const ID_TYPE id = payload;
    store_->stages.remove(id);
    this->stageRemoved(id, getStore());

    /*
     * GROUPS
     */
  } else if (event == RetrieveEvents::GROUP_ADDED) {
    const auto group = parse<Group>(payload, event, "Group");
    store_->groups.create(payload);
    this->groupAdded(group, getStore());
  } else if (event == RetrieveEvents::GROUP_CHANGED) {
    store_->groups.update(payload);
    const ID_TYPE id = payload["_id"];
    this->groupChanged(id, payload, getStore());
  } else if (event == RetrieveEvents::GROUP_REMOVED) {
    const ID_TYPE id = payload;
    store_->groups.remove(id);
    this->groupRemoved(id, getStore());

    /*
     * CUSTOM GROUP
     */
  } else if (event == RetrieveEvents::CUSTOM_GROUP_ADDED) {
    const auto customGroup = parse<CustomGroup>(payload, event, "CustomGroup");
    store_->customGroups.create(payload);
    this->customGroupAdded(customGroup, getStore());
  } else if (event == RetrieveEvents::CUSTOM_GROUP_CHANGED) {
    store_->customGroups.update(payload);
    const ID_TYPE id = payload["_id"];
    this->customGroupChanged(id, payload, getStore());
  } else if (event == RetrieveEvents::CUSTOM_GROUP_REMOVED) {
    const ID_TYPE id = payload;
    auto custom_group = store_->customGroups.get(id);
    if (custom_group) {
      store_->customGroups.remove(id);
      this->customGroupRemoved(*custom_group, getStore());
    }

    /*
     * STAGE MEMBERS
     */
  } else if (event == RetrieveEvents::STAGE_MEMBER_ADDED) {
    const auto stage_member = parse<StageMember>(payload, event, "StageMember");
    store_->stageMembers.create(payload);
    this->stageMemberAdded(stage_member, getStore());
  } else if (event == RetrieveEvents::STAGE_MEMBER_CHANGED) {
    store_->stageMembers.update(payload);
    const ID_TYPE id = payload["_id"];
    this->stageMemberChanged(id, payload, getStore());
    if (id == this->store_->getStageMemberId()) {
      if (payload.count("groupId") != 0) {
        if (payload["groupId"].is_null()) {
          this->store_->setGroupId(std::nullopt);
        } else {
          this->store_->setGroupId(payload["groupId"].get<ID_TYPE>());
        }
      }
    }
  } else if (event == RetrieveEvents::STAGE_MEMBER_REMOVED) {
    const ID_TYPE id = payload;
    store_->stageMembers.remove(id);
    this->stageMemberRemoved(id, getStore());

    /*
     * STAGE DEVICES
     */
  } else if (event == RetrieveEvents::STAGE_DEVICE_ADDED) {
    const auto stageDevice = parse<StageDevice>(payload, event, "StageDevice");
    store_->stageDevices.create(payload);
    auto localDeviceId = store_->getLocalDeviceId();
    auto stageId = store_->getStageId();
    if (localDeviceId && stageId && *stageId == stageDevice.stageId &&
        *localDeviceId == stageDevice.deviceId) {
      store_->setStageDeviceId(stageDevice._id);
    }
    this->stageDeviceAdded(stageDevice, getStore());
  } else if (event == RetrieveEvents::STAGE_DEVICE_CHANGED) {
    store_->stageDevices.update(payload);
    const ID_TYPE id = payload["_id"];
    this->stageDeviceChanged(id, payload, getStore());
  } else if (event == RetrieveEvents::STAGE_DEVICE_REMOVED) {
    const ID_TYPE id = payload;
    auto stageDevice = store_->stageDevices.get(id);
    if (stageDevice) {
      store_->stageDevices.remove(id);
      auto stageId = store_->getStageId();
      auto localDeviceId = store_->getLocalDeviceId();
      if (localDeviceId && stageId && *stageId == stageDevice->stageId &&
          *localDeviceId == stageDevice->deviceId) {
        store_->resetStageDeviceId();
      }
      this->stageDeviceRemoved(*stageDevice, getStore());
    }

    /*
     * VIDEO TRACKS
     */
  } else if (event == RetrieveEvents::VIDEO_TRACK_ADDED) {
    const auto videoTrack = parse<VideoTrack>(payload, event, "VideoTrack");
    store_->videoTracks.create(payload);
    this->videoTrackAdded(videoTrack,
                          getStore());
  } else if (event == RetrieveEvents::VIDEO_TRACK_CHANGED) {
    store_->videoTracks.update(payload);
    const ID_TYPE id = payload["_id"];
    this->videoTrackChanged(id, payload, getStore());
  } else if (event == RetrieveEvents::VIDEO_TRACK_REMOVED) {
    const ID_TYPE id = payload;
    auto track = store_->videoTracks.get(id);
    store_->videoTracks.remove(id);
    this->videoTrackRemoved(*track, getStore());

    /*
     * AUDIO TRACKS
     */
  } else if (event == RetrieveEvents::AUDIO_TRACK_ADDED) {
    const auto audioTrack = parse<AudioTrack>(payload, event, "AudioTrack");
    store_->audioTracks.create(payload);
    this->audioTrackAdded(audioTrack,
                          getStore());
  } else if (event == RetrieveEvents::AUDIO_TRACK_CHANGED) {
    store_->audioTracks.update(payload);
    const ID_TYPE id = payload["_id"];
    this->audioTrackChanged(id, payload, getStore());
  } else if (event == RetrieveEvents::AUDIO_TRACK_REMOVED) {
    const ID_TYPE id = payload;
    auto track = store_->audioTracks.get(id);
    store_->audioTracks.remove(id);
    this->audioTrackRemoved(*track, getStore());

    /*
     * USERS
     */
  } else if (event == RetrieveEvents::USER_ADDED) {
    const auto user = parse<User>(payload, event, "User");
    store_->users.create(payload);
    this->userAdded(user, getStore());
  } else if (event == RetrieveEvents::USER_CHANGED) {
    store_->users.update(payload);
    const ID_TYPE id = payload["_id"];
    this->userChanged(id, payload, getStore());
  } else if (event == RetrieveEvents::USER_REMOVED) {
    const ID_TYPE id = payload;
    store_->users.remove(id);
    this->userRemoved(id, getStore());

    /*
     * SOUND CARD
     */
  } else if (event == RetrieveEvents::SOUND_CARD_ADDED) {
    const auto soundCard = parse<SoundCard>(payload, event, "SoundCard");
    store_->soundCards.create(payload);
    this->soundCardAdded(soundCard, getStore());
  } else if (event == RetrieveEvents::SOUND_CARD_CHANGED) {
    store_->soundCards.update(payload);
    const ID_TYPE id = payload["_id"];
    this->soundCardChanged(id, payload, getStore());
    auto localDevice = store_->getLocalDevice();
    if (localDevice) {
      if (localDevice->inputSoundCardId == id) {
        this->inputSoundCardChanged(id, payload, getStore());
      }
      if (localDevice->outputSoundCardId == id) {
        this->outputSoundCardChanged(id, payload, getStore());
      }
    }
  } else if (event == RetrieveEvents::SOUND_CARD_REMOVED) {
    const ID_TYPE id = payload;
    store_->soundCards.remove(id);
    this->soundCardRemoved(id, getStore());

    /*
     * STAGE JOINED
     */
  } else if (event == RetrieveEvents::STAGE_JOINED) {
    //TODO: Wrap
    auto stageId = payload["stageId"].get<std::string>();
    auto stageMemberId = payload["stageMemberId"].get<std::string>();
    auto groupId = payload["groupId"].is_null() ? std::nullopt : std::optional<std::string>(payload["groupId"].get<
        std::string>());
    auto localDeviceId = store_->getLocalDeviceId();
    if (payload.count("remoteUsers") > 0) {
      for (const auto &item: payload["remoteUsers"]) {
        const auto user = parse<User>(item, event, "User");
        store_->users.create(item);
        this->userAdded(user, getStore());
      }
    }
    if (payload.count("stage") > 0) {
      store_->stages.create(payload["stage"]);
      this->stageAdded(payload["stage"].get<Stage>(), getStore());
    }
    if (payload.count("groups") > 0) {
      for (const auto &item: payload["groups"]) {
        const auto group = parse<Group>(item, event, "Group");
        store_->groups.create(item);
        this->groupAdded(group, getStore());
      }
    }
    if (payload.contains("customGroups")) {
      for (const auto &item: payload["customGroups"]) {
        store_->customGroups.create(item);
        this->customGroupAdded(
            item.get<CustomGroup>(), getStore());
      }
    }
    for (const auto &item: payload["stageMembers"]) {
      store_->stageMembers.create(item);
      auto stage_member = item.get<StageMember>();
      this->stageMemberAdded(stage_member, getStore());
    }
    for (const auto &item: payload["stageDevices"]) {
      store_->stageDevices.create(item);
      auto stageDevice = item.get<StageDevice>();
      if (localDeviceId && stageId == stageDevice.stageId &&
          *localDeviceId == stageDevice.deviceId) {
        store_->setStageDeviceId(stageDevice._id);
      }
      this->stageDeviceAdded(stageDevice, getStore());
    }
    for (const auto &item: payload["audioTracks"]) {
      store_->audioTracks.create(item);
      this->audioTrackAdded(item.get<AudioTrack>(),
                            getStore());
    }
    for (const auto &item: payload["videoTracks"]) {
      store_->videoTracks.create(item);
      this->videoTrackAdded(item.get<VideoTrack>(),
                            getStore());
    }
    store_->setStageId(stageId);
    store_->setGroupId(groupId);
    store_->setStageMemberId(stageMemberId);
    this->stageJoined(stageId, groupId, getStore());

    /*
     * STAGE LEFT
     */
  } else if (event == RetrieveEvents::STAGE_LEFT) {
    store_->resetStageId();
    store_->resetGroupId();
    store_->resetStageMemberId();
    store_->resetStageDeviceId();
    store_->stageMembers.removeAll();
    //store_->customGroups.removeAll();
    store_->videoTracks.removeAll();
    store_->audioTracks.removeAll();
    // TODO: Discuss, the store may dispatch all the events instead...
    // TODO: Otherwise we have to dispatch all removals HERE (!)
    // Current workaround: assuming, that on left all using
    // components know, that the entities are removed without event
    this->stageLeft(getStore());

    // WebRTC
  } else if (event == RetrieveEvents::P2P_RESTART) {
    const auto item = parse<P2PRestart>(payload, event, "P2PRestart");
    this->p2pRestart(item, getStore());
  } else if (event == RetrieveEvents::P2P_OFFER_SENT) {
    const auto item = parse<P2POffer>(payload, event, "P2POffer");
    this->p2pOffer(item, getStore());
  } else if (event == RetrieveEvents::P2P_ANSWER_SENT) {
    const auto item = parse<P2PAnswer>(payload, event, "P2PAnswer");
    this->p2pAnswer(item, getStore());
  } else if (event == RetrieveEvents::ICE_CANDIDATE_SENT) {
    const auto item = parse<IceCandidate>(payload, event, "IceCandidate");
    this->iceCandidate(item, getStore());
  }
}
