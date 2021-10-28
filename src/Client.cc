#include "DigitalStage/Api/Client.h"
#include "DigitalStage/Api/Events.h"

#include <exception>
#include <iostream>
#include <memory>
#include <utility>

using namespace DigitalStage::Api;

Client::Client(const std::string &apiUrl)
    : apiUrl_(apiUrl) {
  store_ = std::make_unique<Store>();
  wsclient_ = std::make_unique<teckos::client>();
  wsclient_->setReconnect(true);
  wsclient_->sendPayloadOnReconnect(true);
}

Store *Client::getStore() const {
  return this->store_.get();
}

void Client::disconnect() {
  if (wsclient_)
    wsclient_->disconnect();
}

bool Client::isConnected() {
  if (wsclient_)
    wsclient_->isConnected();
  return false;
}

pplx::task<void> Client::connect(const teckos::string_t &apiToken,
                                 const nlohmann::json &initialDevice) {
  std::cout << "Connecting to " << apiUrl_ << std::endl;

  // Set handler
  wsclient_->setMessageHandler([&](const nlohmann::json &j) {
    try {
      if (!j.is_array()) {
        std::cerr << "WARNING: not an array: " << j.dump() << std::endl;
        return;
      }
      const std::string &event = j[0];
      const nlohmann::json payload = (j.size() > 1) ? j[1] : nlohmann::json::object();

#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
      std::cout << "[EVENT] " << event << " " << payload.dump() << std::endl;
#else
      std::cout << "[EVENT] " << event << std::endl;
#endif
#endif

      if (event == RetrieveEvents::READY) {
        std::cout << "READY" << std::endl;
        store_->setReady(true);
        this->ready(getStore());

        /*
         * LOCAL DEVICE
         */
      } else if (event == RetrieveEvents::LOCAL_DEVICE_READY) {
        const auto device = payload.get<Device>();
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
        const auto user = payload.get<User>();
        store_->users.create(payload);
        store_->setUserId(user._id);
        this->userAdded(user, getStore());
        this->localUserReady(user, getStore());

        /*
         * DEVICES
         */
      } else if (event == RetrieveEvents::DEVICE_ADDED) {
        store_->devices.create(payload);
        auto device = payload.get<Device>();
        this->deviceAdded(device, getStore());
      } else if (event == RetrieveEvents::DEVICE_CHANGED) {
        store_->devices.update(payload);
        const std::string id = payload["_id"];
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
        const std::string id = payload;
        store_->devices.remove(id);
        this->deviceRemoved(id, getStore());

        /*
         * STAGE
         */
      } else if (event == RetrieveEvents::STAGE_ADDED) {
        store_->stages.create(payload);
        this->stageAdded(payload.get<Stage>(), getStore());
      } else if (event == RetrieveEvents::STAGE_CHANGED) {
        store_->stages.update(payload);
        const std::string id = payload["_id"];
        this->stageChanged(id, payload, getStore());
      } else if (event == RetrieveEvents::STAGE_REMOVED) {
        const std::string id = payload;
        store_->stages.remove(id);
        this->stageRemoved(id, getStore());

        /*
         * GROUPS
         */
      } else if (event == RetrieveEvents::GROUP_ADDED) {
        store_->groups.create(payload);
        this->groupAdded(payload.get<Group>(), getStore());
      } else if (event == RetrieveEvents::GROUP_CHANGED) {
        store_->groups.update(payload);
        const std::string id = payload["_id"];
        this->groupChanged(id, payload, getStore());
      } else if (event == RetrieveEvents::GROUP_REMOVED) {
        const std::string id = payload;
        store_->groups.remove(id);
        this->groupRemoved(id, getStore());

        /*
         * CUSTOM GROUP VOLUME AND POSITIONS
         */
      } else if (event == RetrieveEvents::CUSTOM_GROUP_POSITION_ADDED) {
        store_->customGroupPositions.create(payload);
        this->customGroupPositionAdded(payload.get<CustomGroupPosition>(),
                                       getStore());
      } else if (event == RetrieveEvents::CUSTOM_GROUP_POSITION_CHANGED) {
        store_->customGroupPositions.update(payload);
        const std::string id = payload["_id"];
        this->customGroupPositionChanged(id, payload, getStore());
      } else if (event == RetrieveEvents::CUSTOM_GROUP_POSITION_REMOVED) {
        const std::string id = payload;
        store_->customGroupPositions.remove(id);
        this->customGroupPositionRemoved(id, getStore());
      } else if (event == RetrieveEvents::CUSTOM_GROUP_VOLUME_ADDED) {
        store_->customGroupVolumes.create(payload);
        this->customGroupVolumeAdded(payload.get<CustomGroupVolume>(),
                                     getStore());
      } else if (event == RetrieveEvents::CUSTOM_GROUP_VOLUME_CHANGED) {
        store_->customGroupVolumes.update(payload);
        const std::string id = payload["_id"];
        this->customGroupVolumeChanged(id, payload, getStore());
      } else if (event == RetrieveEvents::CUSTOM_GROUP_VOLUME_REMOVED) {
        const std::string id = payload;
        store_->customGroupVolumes.remove(id);
        this->customGroupVolumeRemoved(id, getStore());

        /*
         * STAGE MEMBERS
         */
      } else if (event == RetrieveEvents::STAGE_MEMBER_ADDED) {
        store_->stageMembers.create(payload);
        this->stageMemberAdded(payload.get<StageMember>(), getStore());
      } else if (event == RetrieveEvents::STAGE_MEMBER_CHANGED) {
        store_->stageMembers.update(payload);
        const std::string id = payload["_id"];
        this->stageMemberChanged(id, payload, getStore());
      } else if (event == RetrieveEvents::STAGE_MEMBER_REMOVED) {
        const std::string id = payload;
        store_->stageMembers.remove(id);
        this->stageMemberRemoved(id, getStore());

        /*
         * CUSTOM STAGE MEMBERS VOLUME AND POSITIONS
         */
      } else if (event == RetrieveEvents::CUSTOM_STAGE_MEMBER_POSITION_ADDED) {
        store_->customStageMemberPositions.create(payload);
        this->customStageMemberPositionAdded(
            payload.get<CustomStageMemberPosition>(), getStore());
      } else if (event == RetrieveEvents::CUSTOM_STAGE_MEMBER_POSITION_CHANGED) {
        store_->customStageMemberPositions.update(payload);
        const std::string id = payload["_id"];
        this->customStageMemberPositionChanged(id, payload, getStore());
      } else if (event == RetrieveEvents::CUSTOM_STAGE_MEMBER_POSITION_REMOVED) {
        const std::string id = payload;
        store_->customStageMemberPositions.remove(id);
        this->customStageMemberPositionRemoved(id, getStore());
      } else if (event == RetrieveEvents::CUSTOM_STAGE_MEMBER_VOLUME_ADDED) {
        store_->customStageMemberVolumes.create(payload);
        this->customStageMemberVolumeAdded(
            payload.get<CustomStageMemberVolume>(), getStore());
      } else if (event == RetrieveEvents::CUSTOM_STAGE_MEMBER_VOLUME_CHANGED) {
        store_->customStageMemberVolumes.update(payload);
        const std::string id = payload["_id"];
        this->customStageMemberVolumeChanged(id, payload, getStore());
      } else if (event == RetrieveEvents::CUSTOM_STAGE_MEMBER_VOLUME_REMOVED) {
        const std::string id = payload;
        store_->customStageMemberVolumes.remove(id);
        this->customStageMemberVolumeRemoved(id, getStore());

        /*
         * STAGE DEVICES
         */
      } else if (event == RetrieveEvents::STAGE_DEVICE_ADDED) {
        store_->stageDevices.create(payload);
        auto stageDevice = payload.get<StageDevice>();
        auto localDeviceId = store_->getLocalDeviceId();
        auto stageId = store_->getStageId();
        if (localDeviceId && stageId && *stageId == stageDevice.stageId &&
            *localDeviceId == stageDevice.deviceId) {
          store_->setStageDeviceId(stageDevice._id);
        }
        this->stageDeviceAdded(stageDevice, getStore());
      } else if (event == RetrieveEvents::STAGE_DEVICE_CHANGED) {
        store_->stageDevices.update(payload);
        const std::string id = payload["_id"];
        this->stageDeviceChanged(id, payload, getStore());
      } else if (event == RetrieveEvents::STAGE_DEVICE_REMOVED) {
        const std::string id = payload;
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
         * CUSTOM STAGE DEVICES VOLUME AND POSITIONS
         */
      } else if (event == RetrieveEvents::CUSTOM_STAGE_DEVICE_POSITION_ADDED) {
        store_->customStageDevicePositions.create(payload);
        this->customStageDevicePositionAdded(
            payload.get<CustomStageDevicePosition>(), getStore());
      } else if (event == RetrieveEvents::CUSTOM_STAGE_DEVICE_POSITION_CHANGED) {
        store_->customStageDevicePositions.update(payload);
        const std::string id = payload["_id"];
        this->customStageDevicePositionChanged(id, payload, getStore());
      } else if (event == RetrieveEvents::CUSTOM_STAGE_DEVICE_POSITION_REMOVED) {
        const std::string id = payload;
        store_->customStageDevicePositions.remove(id);
        this->customStageDevicePositionRemoved(id, getStore());
      } else if (event == RetrieveEvents::CUSTOM_STAGE_DEVICE_VOLUME_ADDED) {
        store_->customStageDeviceVolumes.create(payload);
        this->customStageDeviceVolumeAdded(
            payload.get<CustomStageDeviceVolume>(), getStore());
      } else if (event == RetrieveEvents::CUSTOM_STAGE_DEVICE_VOLUME_CHANGED) {
        store_->customStageDeviceVolumes.update(payload);
        const std::string id = payload["_id"];
        this->customStageDeviceVolumeChanged(id, payload, getStore());
      } else if (event == RetrieveEvents::CUSTOM_STAGE_DEVICE_VOLUME_REMOVED) {
        const std::string id = payload;
        store_->customStageDeviceVolumes.remove(id);
        this->customStageDeviceVolumeRemoved(id, getStore());

        /*
         * VIDEO TRACKS
         */
      } else if (event == RetrieveEvents::VIDEO_TRACK_ADDED) {
        store_->videoTracks.create(payload);
        this->videoTrackAdded(payload.get<VideoTrack>(),
                              getStore());
      } else if (event == RetrieveEvents::VIDEO_TRACK_CHANGED) {
        store_->videoTracks.update(payload);
        const std::string id = payload["_id"];
        this->videoTrackChanged(id, payload, getStore());
      } else if (event == RetrieveEvents::VIDEO_TRACK_REMOVED) {
        const std::string id = payload;
        auto track = store_->videoTracks.get(id);
        store_->videoTracks.remove(id);
        this->videoTrackRemoved(*track, getStore());

        /*
         * AUDIO TRACKS
         */
      } else if (event == RetrieveEvents::AUDIO_TRACK_ADDED) {
        store_->audioTracks.create(payload);
        this->audioTrackAdded(payload.get<AudioTrack>(),
                              getStore());
      } else if (event == RetrieveEvents::AUDIO_TRACK_CHANGED) {
        store_->audioTracks.update(payload);
        const std::string id = payload["_id"];
        this->audioTrackChanged(id, payload, getStore());
      } else if (event == RetrieveEvents::AUDIO_TRACK_REMOVED) {
        const std::string id = payload;
        auto track = store_->audioTracks.get(id);
        store_->audioTracks.remove(id);
        this->audioTrackRemoved(*track, getStore());

        /*
         * AUDIO TRACK VOLUME AND POSITIONS
         */
      } else if (event ==
          RetrieveEvents::CUSTOM_AUDIO_TRACK_POSITION_ADDED) {
        store_->customAudioTrackPositions.create(payload);
        this->customAudioTrackPositionAdded(
            payload.get<CustomAudioTrackPosition>(), getStore());
      } else if (event ==
          RetrieveEvents::CUSTOM_AUDIO_TRACK_POSITION_CHANGED) {
        store_->customAudioTrackPositions.update(payload);
        const std::string id = payload["_id"];
        this->customAudioTrackPositionChanged(id, payload, getStore());
      } else if (event ==
          RetrieveEvents::CUSTOM_AUDIO_TRACK_POSITION_REMOVED) {
        const std::string id = payload;
        store_->customAudioTrackPositions.remove(id);
        this->customAudioTrackPositionRemoved(id, getStore());
      } else if (event ==
          RetrieveEvents::CUSTOM_AUDIO_TRACK_VOLUME_ADDED) {
        store_->customAudioTrackVolumes.create(payload);
        this->customAudioTrackVolumeAdded(
            payload.get<CustomAudioTrackVolume>(), getStore());
      } else if (event ==
          RetrieveEvents::CUSTOM_AUDIO_TRACK_VOLUME_CHANGED) {
        store_->customAudioTrackVolumes.update(payload);
        const std::string id = payload["_id"];
        this->customAudioTrackVolumeChanged(id, payload, getStore());
      } else if (event ==
          RetrieveEvents::CUSTOM_AUDIO_TRACK_VOLUME_REMOVED) {
        const std::string id = payload;
        store_->customAudioTrackVolumes.remove(id);
        this->customAudioTrackVolumeRemoved(id, getStore());

        /*
         * USERS
         */
      } else if (event == RetrieveEvents::REMOTE_USER_ADDED) {
        store_->users.create(payload);
        this->userAdded(payload.get<User>(), getStore());
      } else if (event == RetrieveEvents::REMOTE_USER_CHANGED) {
        store_->users.update(payload);
        const std::string id = payload["_id"];
        this->userChanged(id, payload, getStore());
      } else if (event == RetrieveEvents::REMOTE_USER_REMOVED) {
        const std::string id = payload;
        store_->users.remove(id);
        this->userRemoved(id, getStore());

        /*
         * SOUND CARD
         */
      } else if (event == RetrieveEvents::SOUND_CARD_ADDED) {
        store_->soundCards.create(payload);
        this->soundCardAdded(payload.get<SoundCard>(), getStore());
      } else if (event == RetrieveEvents::SOUND_CARD_CHANGED) {
        store_->soundCards.update(payload);
        const std::string id = payload["_id"];
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
        const std::string id = payload;
        store_->soundCards.remove(id);
        this->soundCardRemoved(id, getStore());

        /*
         * STAGE JOINED
         */
      } else if (event == RetrieveEvents::STAGE_JOINED) {
        auto stageId = payload["stageId"].get<std::string>();
        auto groupId = payload["groupId"].get<std::string>();
        auto localDeviceId = store_->getLocalDeviceId();
        if (payload.count("remoteUsers") > 0) {
          for (const auto &item: payload["remoteUsers"]) {
            store_->users.create(item);
            this->userAdded(item.get<User>(), getStore());
          }
        }
        if (payload.count("stage") > 0) {
          for (const auto &item: payload["stage"]) {
            store_->stages.create(item);
            this->stageAdded(item.get<Stage>(), getStore());
          }
        }
        if (payload.count("groups") > 0) {
          for (const auto &item: payload["groups"]) {
            store_->groups.create(item);
            this->groupAdded(item.get<Group>(), getStore());
          }
        }
        for (const auto &item: payload["customGroupVolumes"]) {
          store_->customGroupVolumes.create(item);
          this->customGroupVolumeAdded(item.get<CustomGroupVolume>(),
                                       getStore());
        }
        for (const auto &item: payload["customGroupPositions"]) {
          store_->customGroupPositions.create(item);
          this->customGroupPositionAdded(item.get<CustomGroupPosition>(),
                                         getStore());
        }
        for (const auto &item: payload["stageMembers"]) {
          store_->stageMembers.create(item);
          this->stageMemberAdded(item.get<StageMember>(), getStore());
        }
        for (const auto &item: payload["customStageMemberVolumes"]) {
          store_->customStageMemberVolumes.create(item);
          this->customStageMemberVolumeAdded(
              item.get<CustomStageMemberVolume>(), getStore());
        }
        for (const auto &item: payload["customStageMemberPositions"]) {
          store_->customStageMemberPositions.create(item);
          this->customStageMemberPositionAdded(
              item.get<CustomStageMemberPosition>(), getStore());
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
        for (const auto &item: payload["customStageDeviceVolumes"]) {
          store_->customStageDeviceVolumes.create(item);
          this->customStageDeviceVolumeAdded(
              item.get<CustomStageDeviceVolume>(), getStore());
        }
        for (const auto &item: payload["customStageDevicePositions"]) {
          store_->customStageDevicePositions.create(item);
          this->customStageDevicePositionAdded(
              item.get<CustomStageDevicePosition>(), getStore());
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
        for (const auto &item: payload["customAudioTrackPositions"]) {
          store_->customAudioTrackPositions.create(item);
          this->customAudioTrackPositionAdded(
              item.get<CustomAudioTrackPosition>(), getStore());
        }
        for (const auto &item: payload["customAudioTrackVolumes"]) {
          store_->customAudioTrackVolumes.create(item);
          this->customAudioTrackVolumeAdded(
              item.get<CustomAudioTrackVolume>(), getStore());
        }
        store_->setStageId(stageId);
        store_->setGroupId(groupId);
        this->stageJoined(stageId, groupId, getStore());

        /*
         * STAGE LEFT
         */
      } else if (event == RetrieveEvents::STAGE_LEFT) {
        store_->resetStageId();
        store_->resetGroupId();
        store_->resetStageDeviceId();
        store_->stageMembers.removeAll();
        store_->customStageMemberPositions.removeAll();
        store_->customStageMemberVolumes.removeAll();
        store_->customGroupPositions.removeAll();
        store_->customGroupVolumes.removeAll();
        store_->stageMembers.removeAll();
        store_->customStageMemberPositions.removeAll();
        store_->customStageMemberVolumes.removeAll();
        store_->videoTracks.removeAll();
        store_->audioTracks.removeAll();
        store_->customAudioTrackPositions.removeAll();
        store_->customAudioTrackVolumes.removeAll();
        std::cout << "See me?" << event << std::endl;
        // TODO: Discuss, the store may dispatch all the events instead...
        // TODO: Otherwise we have to dispatch all removals HERE (!)
        // Current workaround: assuming, that on left all using
        // components know, that the entities are removed without event
        this->stageLeft(getStore());

        // WebRTC
      } else if (event == RetrieveEvents::P2P_RESTART) {
        this->p2pRestart(payload.get<P2PRestart>(), getStore());
      } else if (event == RetrieveEvents::P2P_OFFER_SENT) {
        this->p2pOffer(payload.get<P2POffer>(), getStore());
      } else if (event == RetrieveEvents::P2P_ANSWER_SENT) {
        this->p2pAnswer(payload.get<P2PAnswer>(), getStore());
      } else if (event == RetrieveEvents::ICE_CANDIDATE_SENT) {
        this->iceCandidate(payload.get<IceCandidate>(), getStore());

      } else {
        std::cerr << "Unknown event " << event << std::endl;
      }
    }
    catch (const std::exception &e) {
      std::cerr << "[ERROR] std::exception: " << e.what() << std::endl;
    }
    catch (...) {
      std::cerr << "[ERROR] error parsing" << std::endl;
    }
  });

  teckos::string_t apiUrl(this->apiUrl_.begin(), this->apiUrl_.end());
  return wsclient_->connect(apiUrl, apiToken,
                            {{"device", initialDevice}});
}

pplx::task<void> Client::send(const std::string &event,
                              const nlohmann::json &message) {
#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
  std::cout << "[SENDING] " << event << ": " << message << std::endl;
#else
  std::cout << "[SENDING] " << event << std::endl;
#endif
#endif
  if (!wsclient_)
    throw std::runtime_error("Not ready");
  return wsclient_->emit(event, message);
}

pplx::task<void> Client::send(
    const std::string &event, const nlohmann::json &message,
    const std::function<void(const std::vector<nlohmann::json> &)> &callback) {
#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
  std::cout << "[SENDING] " << event << ": " << message << std::endl;
#else
  std::cout << "[SENDING] " << event << std::endl;
#endif
#endif
  if (!wsclient_)
    throw std::runtime_error("Not ready");
  return wsclient_->emit(event, message, callback);
}

[[maybe_unused]] DigitalStage::Types::WholeStage Client::getWholeStage() const {
  const std::lock_guard<std::mutex> lock(wholeStage_mutex_);
  return wholeStage_.get<DigitalStage::Types::WholeStage>();
}

[[maybe_unused]] void Client::setWholeStage(nlohmann::json wholeStage) {
  const std::lock_guard<std::mutex> lock(wholeStage_mutex_);
  this->wholeStage_ = std::move(wholeStage);
}