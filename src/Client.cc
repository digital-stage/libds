#include "DigitalStage/Api/Client.h"
#include "DigitalStage/Api/Events.h"

#include <exception>
#include <iostream>
#include <memory>
#include <utility>

using namespace DigitalStage::Api;

Client::Client(std::string apiUrl) : apiUrl_(std::move(apiUrl))
{
  store_ = std::make_unique<Store>();
  wsclient_ = std::make_unique<teckos::client>();
  wsclient_->setReconnect(true);
  wsclient_->sendPayloadOnReconnect(true);
}

Store* Client::getStore() const
{
  return this->store_.get();
}

void Client::disconnect()
{
  if(wsclient_)
    wsclient_->disconnect();
}

bool Client::isConnected()
{
  if(wsclient_)
    wsclient_->isConnected();
  return false;
}

pplx::task<void> Client::connect(const std::string& apiToken,
                                 const nlohmann::json& initialDevice)
{
  std::cout << "Connecting to " << apiUrl_ << std::endl;

  // Set handler
  wsclient_->setMessageHandler([&](const nlohmann::json& j) {
    try {
      if(!j.is_array()) {
        std::cerr << "WARNING: not an array: " << j.dump() << std::endl;
        return;
      }
      const std::string& event = j[0];
      const nlohmann::json payload = j[1];

#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
      std::cout << "[EVENT] " << event << " " << payload.dump() << std::endl;
#else
      std::cout << "[EVENT] " << event << std::endl;
#endif
#endif

      if(event == RetrieveEvents::READY) {
        std::cout << "READY" << std::endl;
        store_->setReady(true);
        this->ready(getStore());

        /*
         * LOCAL DEVICE
         */
      } else if(event == RetrieveEvents::LOCAL_DEVICE_READY) {
        const auto device = payload.get<Device>();
        store_->createDevice(payload);
        store_->setLocalDeviceId(device._id);
        this->deviceAdded(device, getStore());
        this->localDeviceReady(device, getStore());

        /*
         * LOCAL USER
         */
      } else if(event == RetrieveEvents::USER_READY) {
        const auto user = payload.get<User>();
        store_->createUser(payload);
        store_->setUserId(user._id);
        this->userAdded(user, getStore());
        this->localUserReady(user, getStore());

        /*
         * DEVICES
         */
      } else if(event == RetrieveEvents::DEVICE_ADDED) {
        store_->createDevice(payload);
        this->deviceAdded(payload.get<Device>(), getStore());
      } else if(event == RetrieveEvents::DEVICE_CHANGED) {
        store_->updateDevice(payload);
        const std::string id = payload["_id"];
        this->deviceChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::DEVICE_REMOVED) {
        const std::string id = payload;
        store_->removeDevice(id);
        this->deviceRemoved(id, getStore());

        /*
         * STAGE
         */
      } else if(event == RetrieveEvents::STAGE_ADDED) {
        store_->createStage(payload);
        this->stageAdded(payload.get<Stage>(), getStore());
      } else if(event == RetrieveEvents::STAGE_CHANGED) {
        store_->updateStage(payload);
        const std::string id = payload["_id"];
        this->stageChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::STAGE_REMOVED) {
        const std::string id = payload;
        store_->removeStage(id);
        this->stageRemoved(id, getStore());

        /*
         * GROUPS
         */
      } else if(event == RetrieveEvents::GROUP_ADDED) {
        store_->createGroup(payload);
        this->groupAdded(payload.get<Group>(), getStore());
      } else if(event == RetrieveEvents::GROUP_CHANGED) {
        store_->updateGroup(payload);
        const std::string id = payload["_id"];
        this->groupChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::GROUP_REMOVED) {
        const std::string id = payload;
        store_->removeGroup(id);
        this->groupRemoved(id, getStore());

        /*
         * CUSTOM GROUP VOLUME AND POSITIONS
         */
      } else if(event == RetrieveEvents::CUSTOM_GROUP_POSITION_ADDED) {
        store_->createCustomGroupPosition(payload);
        this->customGroupPositionAdded(payload.get<CustomGroupPosition>(),
                                       getStore());
      } else if(event == RetrieveEvents::CUSTOM_GROUP_POSITION_CHANGED) {
        store_->updateCustomGroupPosition(payload);
        const std::string id = payload["_id"];
        this->customGroupPositionChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::CUSTOM_GROUP_POSITION_REMOVED) {
        const std::string id = payload;
        store_->removeCustomGroupPosition(id);
        this->customGroupPositionRemoved(id, getStore());
      } else if(event == RetrieveEvents::CUSTOM_GROUP_VOLUME_ADDED) {
        store_->createCustomGroupVolume(payload);
        this->customGroupVolumeAdded(payload.get<CustomGroupVolume>(),
                                     getStore());
      } else if(event == RetrieveEvents::CUSTOM_GROUP_VOLUME_CHANGED) {
        store_->updateCustomGroupVolume(payload);
        const std::string id = payload["_id"];
        this->customGroupVolumeChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::CUSTOM_GROUP_VOLUME_REMOVED) {
        const std::string id = payload;
        store_->removeCustomGroupVolume(id);
        this->customGroupVolumeRemoved(id, getStore());

        /*
         * STAGE MEMBERS
         */
      } else if(event == RetrieveEvents::STAGE_MEMBER_ADDED) {
        store_->createStageMember(payload);
        this->stageMemberAdded(payload.get<StageMember>(), getStore());
      } else if(event == RetrieveEvents::STAGE_MEMBER_CHANGED) {
        store_->updateStageMember(payload);
        const std::string id = payload["_id"];
        this->stageMemberChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::STAGE_MEMBER_REMOVED) {
        const std::string id = payload;
        store_->removeStageMember(id);
        this->stageMemberRemoved(id, getStore());

        /*
         * CUSTOM STAGE MEMBERS VOLUME AND POSITIONS
         */
      } else if(event == RetrieveEvents::CUSTOM_STAGE_MEMBER_POSITION_ADDED) {
        store_->createCustomStageMemberPosition(payload);
        this->customStageMemberPositionAdded(
            payload.get<CustomStageMemberPosition>(), getStore());
      } else if(event == RetrieveEvents::CUSTOM_STAGE_MEMBER_POSITION_CHANGED) {
        store_->updateCustomStageMemberPosition(payload);
        const std::string id = payload["_id"];
        this->customStageMemberPositionChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::CUSTOM_STAGE_MEMBER_POSITION_REMOVED) {
        const std::string id = payload;
        store_->removeCustomStageMemberPosition(id);
        this->customStageMemberPositionRemoved(id, getStore());
      } else if(event == RetrieveEvents::CUSTOM_STAGE_MEMBER_VOLUME_ADDED) {
        store_->createCustomStageMemberVolume(payload);
        this->customStageMemberVolumeAdded(
            payload.get<CustomStageMemberVolume>(), getStore());
      } else if(event == RetrieveEvents::CUSTOM_STAGE_MEMBER_VOLUME_CHANGED) {
        store_->updateCustomStageMemberVolume(payload);
        const std::string id = payload["_id"];
        this->customStageMemberVolumeChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::CUSTOM_STAGE_MEMBER_VOLUME_REMOVED) {
        const std::string id = payload;
        store_->removeCustomStageMemberVolume(id);
        this->customStageMemberVolumeRemoved(id, getStore());

        /*
         * STAGE DEVICES
         */
      } else if(event == RetrieveEvents::STAGE_DEVICE_ADDED) {
        store_->createStageDevice(payload);
        auto stageDevice = payload.get<StageDevice>();
        auto localDeviceId = store_->getLocalDeviceId();
        auto stageId = store_->getStageId();
        if(localDeviceId && stageId && *stageId == stageDevice.stageId &&
           *localDeviceId == stageDevice.deviceId) {
          store_->setStageDeviceId(stageDevice._id);
        }
        this->stageDeviceAdded(stageDevice, getStore());
      } else if(event == RetrieveEvents::STAGE_DEVICE_CHANGED) {
        store_->updateStageDevice(payload);
        const std::string id = payload["_id"];
        this->stageDeviceChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::STAGE_DEVICE_REMOVED) {
        const std::string id = payload;
        auto stageDevice = store_->getStageDevice(id);
        if(stageDevice) {
          store_->removeStageDevice(id);
          auto stageId = store_->getStageId();
          auto localDeviceId = store_->getLocalDeviceId();
          if(localDeviceId && stageId && *stageId == stageDevice->stageId &&
             *localDeviceId == stageDevice->deviceId) {
            store_->resetStageDeviceId();
          }
          this->stageDeviceRemoved(*stageDevice, getStore());
        }

        /*
         * CUSTOM STAGE DEVICES VOLUME AND POSITIONS
         */
      } else if(event == RetrieveEvents::CUSTOM_STAGE_DEVICE_POSITION_ADDED) {
        store_->createCustomStageDevicePosition(payload);
        this->customStageDevicePositionAdded(
            payload.get<CustomStageDevicePosition>(), getStore());
      } else if(event == RetrieveEvents::CUSTOM_STAGE_DEVICE_POSITION_CHANGED) {
        store_->updateCustomStageDevicePosition(payload);
        const std::string id = payload["_id"];
        this->customStageDevicePositionChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::CUSTOM_STAGE_DEVICE_POSITION_REMOVED) {
        const std::string id = payload;
        store_->removeCustomStageDevicePosition(id);
        this->customStageDevicePositionRemoved(id, getStore());
      } else if(event == RetrieveEvents::CUSTOM_STAGE_DEVICE_VOLUME_ADDED) {
        store_->createCustomStageDeviceVolume(payload);
        this->customStageDeviceVolumeAdded(
            payload.get<CustomStageDeviceVolume>(), getStore());
      } else if(event == RetrieveEvents::CUSTOM_STAGE_DEVICE_VOLUME_CHANGED) {
        store_->updateCustomStageDeviceVolume(payload);
        const std::string id = payload["_id"];
        this->customStageDeviceVolumeChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::CUSTOM_STAGE_DEVICE_VOLUME_REMOVED) {
        const std::string id = payload;
        store_->removeCustomStageDeviceVolume(id);
        this->customStageDeviceVolumeRemoved(id, getStore());

        /*
         * VIDEO TRACKS
         */
      } else if(event == RetrieveEvents::VIDEO_TRACK_ADDED) {
        store_->createVideoTrack(payload);
        this->videoTrackAdded(payload.get<VideoTrack>(),
                                    getStore());
      } else if(event == RetrieveEvents::VIDEO_TRACK_CHANGED) {
        store_->updateVideoTrack(payload);
        const std::string id = payload["_id"];
        this->videoTrackChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::VIDEO_TRACK_REMOVED) {
        const std::string id = payload;
        auto track = store_->getVideoTrack(id);
        store_->removeVideoTrack(id);
        this->videoTrackRemoved(*track, getStore());

        /*
         * AUDIO TRACKS
         */
      } else if(event == RetrieveEvents::AUDIO_TRACK_ADDED) {
        store_->createAudioTrack(payload);
        this->audioTrackAdded(payload.get<AudioTrack>(),
                                    getStore());
      } else if(event == RetrieveEvents::AUDIO_TRACK_CHANGED) {
        store_->updateAudioTrack(payload);
        const std::string id = payload["_id"];
        this->audioTrackChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::AUDIO_TRACK_REMOVED) {
        const std::string id = payload;
        auto track = store_->getAudioTrack(id);
        store_->removeAudioTrack(id);
        this->audioTrackRemoved(*track, getStore());

        /*
         * AUDIO TRACK VOLUME AND POSITIONS
         */
      } else if(event ==
                RetrieveEvents::CUSTOM_AUDIO_TRACK_POSITION_ADDED) {
        store_->createCustomAudioTrackPosition(payload);
        this->customAudioTrackPositionAdded(
            payload.get<CustomAudioTrackPosition>(), getStore());
      } else if(event ==
                RetrieveEvents::CUSTOM_AUDIO_TRACK_POSITION_CHANGED) {
        store_->updateCustomAudioTrackPosition(payload);
        const std::string id = payload["_id"];
        this->customAudioTrackPositionChanged(id, payload, getStore());
      } else if(event ==
                RetrieveEvents::CUSTOM_AUDIO_TRACK_POSITION_REMOVED) {
        const std::string id = payload;
        store_->removeCustomAudioTrackPosition(id);
        this->customAudioTrackPositionRemoved(id, getStore());
      } else if(event ==
                RetrieveEvents::CUSTOM_AUDIO_TRACK_VOLUME_ADDED) {
        store_->createCustomAudioTrackVolume(payload);
        this->customAudioTrackVolumeAdded(
            payload.get<CustomAudioTrackVolume>(), getStore());
      } else if(event ==
                RetrieveEvents::CUSTOM_AUDIO_TRACK_VOLUME_CHANGED) {
        store_->updateCustomAudioTrackVolume(payload);
        const std::string id = payload["_id"];
        this->customAudioTrackVolumeChanged(id, payload, getStore());
      } else if(event ==
                RetrieveEvents::CUSTOM_AUDIO_TRACK_VOLUME_REMOVED) {
        const std::string id = payload;
        store_->removeCustomAudioTrackVolume(id);
        this->customAudioTrackVolumeRemoved(id, getStore());

        /*
         * USERS
         */
      } else if(event == RetrieveEvents::REMOTE_USER_ADDED) {
        store_->createUser(payload);
        this->userAdded(payload.get<User>(), getStore());
      } else if(event == RetrieveEvents::REMOTE_USER_CHANGED) {
        store_->updateUser(payload);
        const std::string id = payload["_id"];
        this->userChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::REMOTE_USER_REMOVED) {
        const std::string id = payload;
        store_->removeUser(id);
        this->userRemoved(id, getStore());

        /*
         * SOUND CARD
         */
      } else if(event == RetrieveEvents::SOUND_CARD_ADDED) {
        store_->createSoundCard(payload);
        this->soundCardAdded(payload.get<SoundCard>(), getStore());
      } else if(event == RetrieveEvents::SOUND_CARD_CHANGED) {
        store_->updateSoundCard(payload);
        const std::string id = payload["_id"];
        this->soundCardChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::SOUND_CARD_REMOVED) {
        const std::string id = payload;
        store_->removeSoundCard(id);
        this->soundCardRemoved(id, getStore());

        /*
         * STAGE JOINED
         */
      } else if(event == RetrieveEvents::STAGE_JOINED) {
        auto stageId = payload["stageId"].get<std::string>();
        auto groupId = payload["groupId"].get<std::string>();
        auto localDeviceId = store_->getLocalDeviceId();
        if(payload.count("remoteUsers") > 0) {
          for(const auto& item : payload["remoteUsers"]) {
            store_->createUser(item);
            this->userAdded(item.get<User>(), getStore());
          }
        }
        if(payload.count("stage") > 0) {
          for(const auto& item : payload["stage"]) {
            store_->createStage(item);
            this->stageAdded(item.get<Stage>(), getStore());
          }
        }
        if(payload.count("groups") > 0) {
          for(const auto& item : payload["groups"]) {
            store_->createGroup(item);
            this->groupAdded(item.get<Group>(), getStore());
          }
        }
        for(const auto& item : payload["customGroupVolumes"]) {
          store_->createCustomGroupVolume(item);
          this->customGroupVolumeAdded(item.get<CustomGroupVolume>(),
                                       getStore());
        }
        for(const auto& item : payload["customGroupPositions"]) {
          store_->createCustomGroupPosition(item);
          this->customGroupPositionAdded(item.get<CustomGroupPosition>(),
                                         getStore());
        }
        for(const auto& item : payload["stageMembers"]) {
          store_->createStageMember(item);
          this->stageMemberAdded(item.get<StageMember>(), getStore());
        }
        for(const auto& item : payload["customStageMemberVolumes"]) {
          store_->createCustomStageMemberVolume(item);
          this->customStageMemberVolumeAdded(
              item.get<CustomStageMemberVolume>(), getStore());
        }
        for(const auto& item : payload["customStageMemberPositions"]) {
          store_->createCustomStageMemberPosition(item);
          this->customStageMemberPositionAdded(
              item.get<CustomStageMemberPosition>(), getStore());
        }
        for(const auto& item : payload["stageDevices"]) {
          store_->createStageDevice(item);
          auto stageDevice = item.get<StageDevice>();
          if(localDeviceId && stageId == stageDevice.stageId &&
             *localDeviceId == stageDevice.deviceId) {
            store_->setStageDeviceId(stageDevice._id);
          }
          this->stageDeviceAdded(stageDevice, getStore());
        }
        for(const auto& item : payload["customStageDeviceVolumes"]) {
          store_->createCustomStageDeviceVolume(item);
          this->customStageDeviceVolumeAdded(
              item.get<CustomStageDeviceVolume>(), getStore());
        }
        for(const auto& item : payload["customStageDevicePositions"]) {
          store_->createCustomStageDevicePosition(item);
          this->customStageDevicePositionAdded(
              item.get<CustomStageDevicePosition>(), getStore());
        }
        for(const auto& item : payload["audioTracks"]) {
          store_->createAudioTrack(item);
          this->audioTrackAdded(item.get<AudioTrack>(),
                                      getStore());
        }
        for(const auto& item : payload["videoTracks"]) {
          store_->createVideoTrack(item);
          this->videoTrackAdded(item.get<VideoTrack>(),
                                      getStore());
        }
        for(const auto& item : payload["customAudioTrackPositions"]) {
          store_->createCustomAudioTrackPosition(item);
          this->customAudioTrackPositionAdded(
              item.get<CustomAudioTrackPosition>(), getStore());
        }
        for(const auto& item : payload["customAudioTrackVolumes"]) {
          store_->createCustomAudioTrackVolume(item);
          this->customAudioTrackVolumeAdded(
              item.get<CustomAudioTrackVolume>(), getStore());
        }
        store_->setStageId(stageId);
        store_->setGroupId(groupId);
        this->stageJoined(stageId, groupId, getStore());

        /*
         * STAGE LEFT
         */
      } else if(event == RetrieveEvents::STAGE_LEFT) {
        store_->resetStageId();
        store_->resetGroupId();
        store_->resetStageDeviceId();
        store_->removeAllStageMembers();
        store_->removeAllCustomStageMemberPositions();
        store_->removeAllCustomStageMemberVolumes();
        store_->removeAllCustomGroupPositions();
        store_->removeAllCustomGroupVolumes();
        store_->removeAllVideoTracks();
        store_->removeAllAudioTracks();
        store_->removeAllCustomAudioTrackPositions();
        store_->removeAllCustomAudioTrackVolumes();
        std::cout << "See me?" << event << std::endl;
        // TODO: Discuss, the store may dispatch all the events instead...
        // TODO: Otherwise we have to dispatch all removals HERE (!)
        // Current workaround: assuming, that on left all using
        // components know, that the entities are removed without event
        this->stageLeft(getStore());
      } else {
        std::cerr << "Unknown event " << event << std::endl;
      }
    }
    catch(const std::exception& e) {
      std::cerr << "[ERROR] std::exception: " << e.what() << std::endl;
    }
    catch(...) {
      std::cerr << "[ERROR] error parsing" << std::endl;
    }
  });

  return wsclient_->connect(U(this->apiUrl_), apiToken,
                            {{"device", initialDevice}});
}

pplx::task<void> Client::send(const std::string& event,
                              const nlohmann::json& message)
{
#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
  std::cout << "[SENDING] " << event << ": " << message << std::endl;
#else
  std::cout << "[SENDING] " << event << std::endl;
#endif
#endif
  if(!wsclient_)
    throw std::runtime_error("Not ready");
  return wsclient_->emit(event, message);
}

pplx::task<void> Client::send(
    const std::string& event, const nlohmann::json& message,
    const std::function<void(const std::vector<nlohmann::json>&)>& callback)
{
#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
  std::cout << "[SENDING] " << event << ": " << message << std::endl;
#else
  std::cout << "[SENDING] " << event << std::endl;
#endif
#endif
  if(!wsclient_)
    throw std::runtime_error("Not ready");
  return wsclient_->emit(event, message, callback);
}

[[maybe_unused]] DigitalStage::Types::WholeStage Client::getWholeStage() const
{
  const std::lock_guard<std::mutex> lock(wholeStage_mutex_);
  return wholeStage_.get<DigitalStage::Types::WholeStage>();
}

[[maybe_unused]] void Client::setWholeStage(nlohmann::json wholeStage)
{
  const std::lock_guard<std::mutex> lock(wholeStage_mutex_);
  this->wholeStage_ = std::move(wholeStage);
}