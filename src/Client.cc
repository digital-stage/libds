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
      if( !j.is_array() ) {
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
        const auto user = payload.get<user_t>();
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
            payload.get<custom_stage_member_position_t>(), getStore());
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
            payload.get<custom_stage_member_volume_t>(), getStore());
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
        this->stageDeviceAdded(payload.get<StageDevice>(), getStore());
      } else if(event == RetrieveEvents::STAGE_DEVICE_CHANGED) {
        store_->updateStageDevice(payload);
        const std::string id = payload["_id"];
        this->stageDeviceChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::STAGE_DEVICE_REMOVED) {
        const std::string id = payload;
        auto stageDevice = store_->getStageDevice(id);
        store_->removeStageDevice(id);
        this->stageDeviceRemoved(*stageDevice, getStore());

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
         * LOCAL VIDEO TRACKS
         */
      } else if(event == RetrieveEvents::LOCAL_VIDEO_TRACK_ADDED) {
        store_->createLocalVideoTrack(payload);
        this->localVideoTrackAdded(payload.get<local_video_track_t>(),
                                   getStore());
      } else if(event == RetrieveEvents::LOCAL_VIDEO_TRACK_CHANGED) {
        store_->updateLocalVideoTrack(payload);
        const std::string id = payload["_id"];
        this->localVideoTrackChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::LOCAL_VIDEO_TRACK_REMOVED) {
        const std::string id = payload;
        store_->removeLocalVideoTrack(id);
        this->localVideoTrackRemoved(id, getStore());

        /*
         * LOCAL AUDIO TRACKS
         */
      } else if(event == RetrieveEvents::LOCAL_AUDIO_TRACK_ADDED) {
        store_->createLocalAudioTrack(payload);
        this->localAudioTrackAdded(payload.get<local_audio_track_t>(),
                                   getStore());
      } else if(event == RetrieveEvents::LOCAL_AUDIO_TRACK_CHANGED) {
        store_->updateLocalAudioTrack(payload);
        const std::string id = payload["_id"];
        this->localAudioTrackChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::LOCAL_AUDIO_TRACK_REMOVED) {
        const std::string id = payload;
        store_->removeLocalAudioTrack(id);
        this->localAudioTrackRemoved(id, getStore());

        /*
         * REMOTE VIDEO TRACKS
         */
      } else if(event == RetrieveEvents::REMOTE_VIDEO_TRACK_ADDED) {
        store_->createRemoteVideoTrack(payload);
        this->remoteVideoTrackAdded(payload.get<remote_video_track_t>(),
                                    getStore());
      } else if(event == RetrieveEvents::REMOTE_VIDEO_TRACK_CHANGED) {
        store_->updateRemoteVideoTrack(payload);
        const std::string id = payload["_id"];
        this->remoteVideoTrackChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::REMOTE_VIDEO_TRACK_REMOVED) {
        const std::string id = payload;
        auto track = store_->getRemoteVideoTrack(id);
        store_->removeRemoteVideoTrack(id);
        this->remoteVideoTrackRemoved(*track, getStore());

        /*
         * REMOTE AUDIO TRACKS
         */
      } else if(event == RetrieveEvents::REMOTE_AUDIO_TRACK_ADDED) {
        store_->createRemoteAudioTrack(payload);
        this->remoteAudioTrackAdded(payload.get<remote_audio_track_t>(),
                                    getStore());
      } else if(event == RetrieveEvents::REMOTE_AUDIO_TRACK_CHANGED) {
        store_->updateRemoteAudioTrack(payload);
        const std::string id = payload["_id"];
        this->remoteAudioTrackChanged(id, payload, getStore());
      } else if(event == RetrieveEvents::REMOTE_AUDIO_TRACK_REMOVED) {
        const std::string id = payload;
        auto track = store_->getRemoteAudioTrack(id);
        store_->removeRemoteAudioTrack(id);
        this->remoteAudioTrackRemoved(*track, getStore());

        /*
         * CUSTOM REMOTE_AUDIO_TRACK VOLUME AND POSITIONS
         */
      } else if(event ==
                RetrieveEvents::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_ADDED) {
        store_->createCustomRemoteAudioTrackPosition(payload);
        this->customRemoteAudioTrackPositionAdded(
            payload.get<custom_remote_audio_track_position_t>(), getStore());
      } else if(event ==
                RetrieveEvents::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_CHANGED) {
        store_->updateCustomRemoteAudioTrackPosition(payload);
        const std::string id = payload["_id"];
        this->customRemoteAudioTrackPositionChanged(id, payload, getStore());
      } else if(event ==
                RetrieveEvents::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_REMOVED) {
        const std::string id = payload;
        store_->removeCustomRemoteAudioTrackPosition(id);
        this->customRemoteAudioTrackPositionRemoved(id, getStore());
      } else if(event ==
                RetrieveEvents::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_ADDED) {
        store_->createCustomRemoteAudioTrackVolume(payload);
        this->customRemoteAudioTrackVolumeAdded(
            payload.get<custom_remote_audio_track_volume_t>(), getStore());
      } else if(event ==
                RetrieveEvents::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_CHANGED) {
        store_->updateCustomRemoteAudioTrackVolume(payload);
        const std::string id = payload["_id"];
        this->customRemoteAudioTrackVolumeChanged(id, payload, getStore());
      } else if(event ==
                RetrieveEvents::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_REMOVED) {
        const std::string id = payload;
        store_->removeCustomRemoteAudioTrackVolume(id);
        this->customRemoteAudioTrackVolumeRemoved(id, getStore());

        /*
         * USERS
         */
      } else if(event == RetrieveEvents::REMOTE_USER_ADDED) {
        store_->createUser(payload);
        this->userAdded(payload.get<user_t>(), getStore());
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
        if(payload.count("remoteUsers") > 0) {
          for(const auto& item : payload["remoteUsers"]) {
            store_->createUser(item);
            this->userAdded(item.get<user_t>(), getStore());
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
              item.get<custom_stage_member_volume_t>(), getStore());
        }
        for(const auto& item : payload["customStageMemberPositions"]) {
          store_->createCustomStageMemberPosition(item);
          this->customStageMemberPositionAdded(
              item.get<custom_stage_member_position_t>(), getStore());
        }
        for(const auto& item : payload["remoteAudioTracks"]) {
          store_->createRemoteAudioTrack(item);
          this->remoteAudioTrackAdded(item.get<remote_audio_track_t>(),
                                      getStore());
        }
        for(const auto& item : payload["remoteVideoTracks"]) {
          store_->createRemoteVideoTrack(item);
          this->remoteVideoTrackAdded(item.get<remote_video_track_t>(),
                                      getStore());
        }
        for(const auto& item : payload["customRemoteAudioTrackPositions"]) {
          store_->createCustomRemoteAudioTrackPosition(item);
          this->customRemoteAudioTrackPositionAdded(
              item.get<custom_remote_audio_track_position_t>(), getStore());
        }
        for(const auto& item : payload["customRemoteAudioTrackVolumes"]) {
          store_->createCustomRemoteAudioTrackVolume(item);
          this->customRemoteAudioTrackVolumeAdded(
              item.get<custom_remote_audio_track_volume_t>(), getStore());
        }
        auto stageId = payload["stageId"].get<std::string>();
        auto groupId = payload["groupId"].get<std::string>();
        store_->setStageId(stageId);
        store_->setGroupId(groupId);
        this->stageJoined(stageId, groupId, getStore());

        /*
         * STAGE LEFT
         */
      } else if(event == RetrieveEvents::STAGE_LEFT) {
        store_->resetStageId();
        store_->resetGroupId();
        store_->removeAllStageMembers();
        std::cout << "See me?" << event << std::endl;
        store_->removeAllCustomStageMemberPositions();
        store_->removeAllCustomStageMemberVolumes();
        store_->removeAllCustomGroupPositions();
        store_->removeAllCustomGroupVolumes();
        store_->removeAllRemoteVideoTracks();
        store_->removeAllRemoteAudioTracks();
        store_->removeAllCustomRemoteAudioTrackPositions();
        store_->removeAllCustomRemoteAudioTrackVolumes();
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