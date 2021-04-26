#include "Client.h"
#include "Events.h"

#include <exception>
#include <iostream>
#include <utility>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace utility::conversions;
using namespace pplx;
using namespace concurrency::streams;

task_completion_event<void> tce;

DigitalStage::Client::Client(const std::string& apiUrl) : apiUrl_(apiUrl)
{
  store_ = std::unique_ptr<DigitalStage::Store>(new DigitalStage::Store());
}

const DigitalStage::Store& DigitalStage::Client::getStore()
{
  return *this->store_;
}

void DigitalStage::Client::disconnect()
{
  tce.set();         // task completion event is set closing wss listening task
  wsclient_.close(); // wss client is closed
}

bool DigitalStage::Client::isConnected()
{
  try {
    return !receiveTask_.is_done();
  }
  catch(...) {
    return false;
  }
}

pplx::task<void>
DigitalStage::Client::connect(const std::string& apiToken,
                              const nlohmann::json initialDevice)
{
  std::cout << "Connecting to " << apiUrl_ << std::endl;
  wsclient_.connect(U(this->apiUrl_)).wait();
  receiveTask_ = create_task(tce);

  // Set handler
  wsclient_.set_message_handler([&](const websocket_incoming_message& ret_msg) {
    auto ret_str = ret_msg.extract_string().get();
    if(ret_str != "hey") {
      try {
        nlohmann::json j = nlohmann::json::parse(ret_str);

        const std::string& event = j["data"][0];
        const nlohmann::json payload = j["data"][1];

#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
        std::cout << "[EVENT] " << event << " " << payload.dump() << std::endl;
#else
        std::cout << "[EVENT] " << event << std::endl;
#endif
#endif

        if(event == WSEvents::READY) {
          this->dispatch(EventType::READY, EventReady{}, getStore());

          /*
           * LOCAL DEVICE
           */
        } else if(event == WSEvents::LOCAL_DEVICE_READY) {
          const device_t device = payload.get<device_t>();
          store_->createDevice(payload);
          store_->setLocalDeviceId(device._id);
          this->dispatch(EventType::LOCAL_DEVICE_READY,
                         EventLocalDeviceReady(device), getStore());
          this->dispatch(EventType::DEVICE_ADDED, EventDeviceAdded(device),
                         getStore());

          /*
           * LOCAL USER
           */
        } else if(event == WSEvents::USER_READY) {
          const user_t user = payload.get<user_t>();
          store_->createUser(payload);
          store_->setUserId(user._id);
          this->dispatch(EventType::USER_READY, EventLocalUserReady(user._id),
                         getStore());
          this->dispatch(EventType::REMOTE_USER_ADDED, EventUserAdded(user),
                         getStore());

          /*
           * DEVICES
           */
        } else if(event == WSEvents::DEVICE_ADDED) {
          device_t device = payload.get<device_t>();
          store_->createDevice(payload);
          this->dispatch(EventType::DEVICE_ADDED, EventDeviceAdded(device),
                         getStore());
        } else if(event == WSEvents::DEVICE_CHANGED) {
          store_->updateDevice(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::DEVICE_CHANGED,
                         EventDeviceChanged(id, payload), getStore());
        } else if(event == WSEvents::DEVICE_REMOVED) {
          const std::string id = payload;
          store_->removeDevice(id);
          // store_->devices_.erase(id);
          this->dispatch(EventType::DEVICE_REMOVED, EventDeviceRemoved(id),
                         getStore());

          /*
           * STAGE
           */
        } else if(event == WSEvents::STAGE_ADDED) {
          stage_t stage = payload.get<stage_t>();
          store_->createStage(payload);
          this->dispatch(EventType::STAGE_ADDED, EventStageAdded(stage),
                         getStore());
        } else if(event == WSEvents::STAGE_CHANGED) {
          store_->updateStage(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::STAGE_CHANGED,
                         EventStageChanged(id, payload), getStore());
        } else if(event == WSEvents::STAGE_REMOVED) {
          const std::string id = payload;
          store_->removeStage(id);
          this->dispatch(EventType::STAGE_REMOVED, EventStageRemoved(id),
                         getStore());

          /*
           * GROUPS
           */
        } else if(event == WSEvents::GROUP_ADDED) {
          store_->createGroup(payload);
          this->dispatch(EventType::GROUP_ADDED,
                         EventGroupAdded(payload.get<group_t>()), getStore());
        } else if(event == WSEvents::GROUP_CHANGED) {
          store_->updateGroup(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::GROUP_CHANGED,
                         EventGroupChanged(id, payload), getStore());
        } else if(event == WSEvents::GROUP_REMOVED) {
          const std::string id = payload;
          store_->removeGroup(id);
          this->dispatch(EventType::GROUP_REMOVED, EventGroupRemoved(id),
                         getStore());

          /*
           * CUSTOM GROUP VOLUME AND POSITIONS
           */
        } else if(event == WSEvents::CUSTOM_GROUP_POSITION_ADDED) {
          store_->createCustomGroupPosition(payload);
          this->dispatch(EventType::CUSTOM_GROUP_POSITION_ADDED,
                         EventCustomGroupPositionAdded(
                             payload.get<custom_group_position_t>()),
                         getStore());
        } else if(event == WSEvents::CUSTOM_GROUP_POSITION_CHANGED) {
          store_->updateCustomGroupPosition(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::CUSTOM_GROUP_POSITION_CHANGED,
                         EventCustomGroupPositionChanged(id, payload),
                         getStore());
        } else if(event == WSEvents::CUSTOM_GROUP_POSITION_REMOVED) {
          const std::string id = payload;
          store_->removeCustomGroupPosition(id);
          this->dispatch(EventType::CUSTOM_GROUP_POSITION_REMOVED,
                         EventCustomGroupPositionRemoved(id), getStore());
        } else if(event == WSEvents::CUSTOM_GROUP_VOLUME_ADDED) {
          store_->createCustomGroupVolume(payload);
          this->dispatch(
              EventType::GROUP_ADDED,
              EventCustomGroupVolumeAdded(payload.get<custom_group_volume_t>()),
              getStore());
        } else if(event == WSEvents::CUSTOM_GROUP_VOLUME_CHANGED) {
          store_->updateCustomGroupVolume(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::CUSTOM_GROUP_VOLUME_CHANGED,
                         EventCustomGroupVolumeChanged(id, payload),
                         getStore());
        } else if(event == WSEvents::CUSTOM_GROUP_VOLUME_REMOVED) {
          const std::string id = payload;
          store_->removeCustomGroupVolume(id);
          this->dispatch(EventType::CUSTOM_GROUP_VOLUME_REMOVED,
                         EventCustomGroupVolumeRemoved(id), getStore());

          /*
           * STAGE MEMBERS
           */
        } else if(event == WSEvents::STAGE_MEMBER_ADDED) {
          store_->createStageMember(payload);
          this->dispatch(EventType::STAGE_MEMBER_ADDED,
                         EventStageMemberAdded(payload.get<stage_member_t>()),
                         getStore());
        } else if(event == WSEvents::STAGE_MEMBER_CHANGED) {
          store_->updateStageMember(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::STAGE_MEMBER_CHANGED,
                         EventStageMemberChanged(id, payload), getStore());
        } else if(event == WSEvents::STAGE_MEMBER_REMOVED) {
          const std::string id = payload;
          store_->removeStageMember(id);
          this->dispatch(EventType::STAGE_MEMBER_REMOVED,
                         EventStageMemberRemoved(id), getStore());

          /*
           * CUSTOM STAGE MEMBERS VOLUME AND POSITIONS
           */
        } else if(event == WSEvents::CUSTOM_STAGE_MEMBER_POSITION_ADDED) {
          store_->createCustomStageMemberPosition(payload);
          this->dispatch(EventType::CUSTOM_STAGE_MEMBER_POSITION_ADDED,
                         EventCustomStageMemberPositionAdded(
                             payload.get<custom_stage_member_position_t>()),
                         getStore());
        } else if(event == WSEvents::CUSTOM_STAGE_MEMBER_POSITION_CHANGED) {
          store_->updateCustomStageMemberPosition(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::CUSTOM_STAGE_MEMBER_POSITION_CHANGED,
                         EventCustomStageMemberPositionChanged(id, payload),
                         getStore());
        } else if(event == WSEvents::CUSTOM_STAGE_MEMBER_POSITION_REMOVED) {
          const std::string id = payload;
          store_->removeCustomStageMemberPosition(id);
          this->dispatch(EventType::CUSTOM_STAGE_MEMBER_POSITION_REMOVED,
                         EventCustomStageMemberPositionRemoved(id), getStore());
        } else if(event == WSEvents::CUSTOM_STAGE_MEMBER_VOLUME_ADDED) {
          store_->createCustomStageMemberVolume(payload);
          this->dispatch(EventType::STAGE_MEMBER_ADDED,
                         EventCustomStageMemberVolumeAdded(
                             payload.get<custom_stage_member_volume_t>()),
                         getStore());
        } else if(event == WSEvents::CUSTOM_STAGE_MEMBER_VOLUME_CHANGED) {
          store_->updateCustomStageMemberVolume(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::CUSTOM_STAGE_MEMBER_VOLUME_CHANGED,
                         EventCustomStageMemberVolumeChanged(id, payload),
                         getStore());
        } else if(event == WSEvents::CUSTOM_STAGE_MEMBER_VOLUME_REMOVED) {
          const std::string id = payload;
          store_->removeCustomStageMemberVolume(id);
          this->dispatch(EventType::CUSTOM_STAGE_MEMBER_VOLUME_REMOVED,
                         EventCustomStageMemberVolumeRemoved(id), getStore());

          /*
           * LOCAL VIDEO TRACKS
           */
        } else if(event == WSEvents::LOCAL_VIDEO_TRACK_ADDED) {
          store_->createLocalVideoTrack(payload);
          this->dispatch(
              EventType::LOCAL_VIDEO_TRACK_ADDED,
              EventLocalVideoTrackAdded(payload.get<local_video_track_t>()),
              getStore());
        } else if(event == WSEvents::LOCAL_VIDEO_TRACK_CHANGED) {
          store_->updateLocalVideoTrack(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::LOCAL_VIDEO_TRACK_CHANGED,
                         EventLocalVideoTrackChanged(id, payload), getStore());
        } else if(event == WSEvents::LOCAL_VIDEO_TRACK_REMOVED) {
          const std::string id = payload;
          store_->removeLocalVideoTrack(id);
          this->dispatch(EventType::LOCAL_VIDEO_TRACK_REMOVED,
                         EventLocalVideoTrackRemoved(id), getStore());

          /*
           * LOCAL AUDIO TRACKS
           */
        } else if(event == WSEvents::LOCAL_AUDIO_TRACK_ADDED) {
          store_->createLocalAudioTrack(payload);
          this->dispatch(
              EventType::LOCAL_AUDIO_TRACK_ADDED,
              EventLocalAudioTrackAdded(payload.get<local_audio_track_t>()),
              getStore());
        } else if(event == WSEvents::LOCAL_AUDIO_TRACK_CHANGED) {
          store_->updateLocalAudioTrack(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::LOCAL_AUDIO_TRACK_CHANGED,
                         EventLocalAudioTrackChanged(id, payload), getStore());
        } else if(event == WSEvents::LOCAL_AUDIO_TRACK_REMOVED) {
          const std::string id = payload;
          store_->removeLocalAudioTrack(id);
          this->dispatch(EventType::LOCAL_AUDIO_TRACK_REMOVED,
                         EventLocalAudioTrackRemoved(id), getStore());

          /*
           * REMOTE VIDEO TRACKS
           */
        } else if(event == WSEvents::REMOTE_VIDEO_TRACK_ADDED) {
          store_->createRemoteVideoTrack(payload);
          this->dispatch(
              EventType::REMOTE_VIDEO_TRACK_ADDED,
              EventRemoteVideoTrackAdded(payload.get<remote_video_track_t>()),
              getStore());
        } else if(event == WSEvents::REMOTE_VIDEO_TRACK_CHANGED) {
          store_->updateRemoteVideoTrack(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::REMOTE_VIDEO_TRACK_CHANGED,
                         EventRemoteAudioTrackChanged(id, payload), getStore());
        } else if(event == WSEvents::REMOTE_VIDEO_TRACK_REMOVED) {
          const std::string id = payload;
          store_->removeRemoteVideoTrack(id);
          this->dispatch(EventType::REMOTE_VIDEO_TRACK_REMOVED,
                         EventRemoteVideoTrackRemoved(id), getStore());

          /*
           * REMOTE AUDIO TRACKS
           */
        } else if(event == WSEvents::REMOTE_AUDIO_TRACK_ADDED) {
          store_->createRemoteAudioTrack(payload);
          this->dispatch(
              EventType::REMOTE_AUDIO_TRACK_ADDED,
              EventRemoteAudioTrackAdded(payload.get<remote_audio_track_t>()),
              getStore());
        } else if(event == WSEvents::REMOTE_AUDIO_TRACK_CHANGED) {
          store_->updateRemoteAudioTrack(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::REMOTE_AUDIO_TRACK_CHANGED,
                         EventRemoteAudioTrackChanged(id, payload), getStore());
        } else if(event == WSEvents::REMOTE_AUDIO_TRACK_REMOVED) {
          const std::string id = payload;
          store_->removeRemoteAudioTrack(id);
          this->dispatch(EventType::REMOTE_AUDIO_TRACK_REMOVED,
                         EventRemoteAudioTrackRemoved(id), getStore());

          /*
           * CUSTOM REMOTE_AUDIO_TRACK VOLUME AND POSITIONS
           */
        } else if(event == WSEvents::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_ADDED) {
          store_->createCustomRemoteAudioTrackPosition(payload);
          this->dispatch(
              EventType::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_ADDED,
              EventCustomRemoteAudioTrackPositionAdded(
                  payload.get<custom_remote_audio_track_position_t>()),
              getStore());
        } else if(event ==
                  WSEvents::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_CHANGED) {
          store_->updateCustomRemoteAudioTrackPosition(payload);
          const std::string id = payload["_id"];
          this->dispatch(
              EventType::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_CHANGED,
              EventCustomRemoteAudioTrackPositionChanged(id, payload),
              getStore());
        } else if(event ==
                  WSEvents::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_REMOVED) {
          const std::string id = payload;
          store_->removeCustomRemoteAudioTrackPosition(id);
          this->dispatch(EventType::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_REMOVED,
                         EventCustomRemoteAudioTrackPositionRemoved(id),
                         getStore());
        } else if(event == WSEvents::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_ADDED) {
          store_->createCustomRemoteAudioTrackVolume(payload);
          this->dispatch(EventType::REMOTE_AUDIO_TRACK_ADDED,
                         EventCustomRemoteAudioTrackVolumeAdded(
                             payload.get<custom_remote_audio_track_volume_t>()),
                         getStore());
        } else if(event == WSEvents::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_CHANGED) {
          store_->updateCustomRemoteAudioTrackVolume(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_CHANGED,
                         EventCustomRemoteAudioTrackVolumeChanged(id, payload),
                         getStore());
        } else if(event == WSEvents::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_REMOVED) {
          const std::string id = payload;
          store_->removeCustomRemoteAudioTrackVolume(id);
          this->dispatch(EventType::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_REMOVED,
                         EventCustomRemoteAudioTrackVolumeRemoved(id),
                         getStore());

          /*
           * USERS
           */
        } else if(event == WSEvents::REMOTE_USER_ADDED) {
          store_->createUser(payload);
          this->dispatch(EventType::REMOTE_USER_ADDED,
                         EventUserAdded(payload.get<user_t>()), getStore());
        } else if(event == WSEvents::REMOTE_USER_CHANGED) {
          store_->updateUser(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::REMOTE_USER_CHANGED,
                         EventUserChanged(id, payload), getStore());
        } else if(event == WSEvents::REMOTE_USER_REMOVED) {
          const std::string id = payload;
          store_->removeUser(id);
          this->dispatch(EventType::REMOTE_USER_REMOVED, EventUserRemoved(id),
                         getStore());

          /*
           * SOUND CARD
           */
        } else if(event == WSEvents::SOUND_CARD_ADDED) {
          store_->createSoundCard(payload);
          this->dispatch(EventType::SOUND_CARD_ADDED,
                         EventSoundCardAdded(payload.get<soundcard_t>()),
                         getStore());
        } else if(event == WSEvents::SOUND_CARD_CHANGED) {
          store_->updateSoundCard(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::SOUND_CARD_ADDED,
                         EventSoundCardChanged(id, payload), getStore());
        } else if(event == WSEvents::SOUND_CARD_REMOVED) {
          const std::string id = payload;
          store_->removeSoundCard(id);
          this->dispatch(EventType::SOUND_CARD_REMOVED,
                         EventSoundCardRemoved(id), getStore());

          /*
           * STAGE JOINED
           */
        } else if(event == WSEvents::STAGE_JOINED) {
          if(payload.count("remoteUsers") > 0) {
            for(const auto& item : payload["remoteUsers"]) {
              store_->createUser(item);
              this->dispatch(EventType::REMOTE_USER_ADDED,
                             EventUserAdded(item.get<user_t>()), getStore());
            }
          }
          if(payload.count("stage") > 0) {
            for(const auto& item : payload["stage"]) {
              store_->createStage(item);
              this->dispatch(EventType::STAGE_ADDED,
                             EventStageAdded(item.get<stage_t>()), getStore());
            }
          }
          if(payload.count("groups") > 0) {
            for(const auto& item : payload["groups"]) {
              store_->createGroup(item);
              this->dispatch(EventType::GROUP_ADDED,
                             EventGroupAdded(item.get<group_t>()), getStore());
            }
          }
          for(const auto& item : payload["customGroupVolumes"]) {
              store_->createCustomGroupVolume(item);
              this->dispatch(EventType::CUSTOM_GROUP_VOLUME_ADDED,
                             EventCustomGroupVolumeAdded(
                                 item.get<custom_group_volume_t>()),
                             getStore());
          }
          for(const auto& item : payload["customGroupPositions"]) {
              store_->createCustomGroupPosition(item);
              this->dispatch(EventType::CUSTOM_GROUP_POSITION_ADDED,
                             EventCustomGroupPositionAdded(
                                 item.get<custom_group_position_t>()),
                             getStore());
          }
          for(const auto& item : payload["stageMembers"]) {
            store_->createStageMember(item);
            this->dispatch(EventType::STAGE_MEMBER_ADDED,
                           EventStageMemberAdded(item.get<stage_member_t>()),
                           getStore());
          }
          for(const auto& item : payload["customStageMemberVolumes"]) {
              store_->createCustomStageMemberVolume(item);
              this->dispatch(EventType::CUSTOM_STAGE_MEMBER_VOLUME_ADDED,
                             EventCustomStageMemberVolumeAdded(
                                 item.get<custom_stage_member_volume_t>()),
                             getStore());
          }
          for(const auto& item : payload["customStageMemberPositions"]) {
              store_->createCustomStageMemberPosition(item);
              this->dispatch(EventType::CUSTOM_STAGE_MEMBER_POSITION_ADDED,
                             EventCustomStageMemberPositionAdded(
                                 item.get<custom_stage_member_position_t>()),
                             getStore());
          }
          for(const auto& item : payload["remoteAudioTracks"]) {
              store_->createRemoteAudioTrack(item);
              this->dispatch(
                  EventType::REMOTE_AUDIO_TRACK_ADDED,
                  EventRemoteAudioTrackAdded(item.get<remote_audio_track_t>()),
                  getStore());
          }
          for(const auto& item : payload["remoteVideoTracks"]) {
              store_->createRemoteVideoTrack(item);
              this->dispatch(
                  EventType::REMOTE_VIDEO_TRACK_ADDED,
                  EventRemoteVideoTrackAdded(item.get<remote_video_track_t>()),
                  getStore());
          }
          for(const auto& item : payload["customRemoteAudioTrackPositions"]) {
              store_->createCustomRemoteAudioTrackPosition(item);
              this->dispatch(
                  EventType::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_ADDED,
                  EventCustomRemoteAudioTrackPositionAdded(
                      item.get<custom_remote_audio_track_position_t>()),
                  getStore());
          }
          for(const auto& item : payload["customRemoteAudioTrackVolumes"]) {
              store_->createCustomRemoteAudioTrackVolume(item);
              this->dispatch(
                  EventType::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_ADDED,
                  EventCustomRemoteAudioTrackVolumeAdded(
                      item.get<custom_remote_audio_track_volume_t>()),
                  getStore());
          }
          auto stageId = payload["stageId"].get<std::string>();
          auto groupId = payload["groupId"].get<std::string>();
          store_->setStageId(stageId);
          store_->setGroupId(groupId);
          this->dispatch(EventType::STAGE_JOINED,
                         EventStageJoined(stageId, groupId), getStore());

          /*
           * STAGE LEFT
           */
        } else if(event == WSEvents::STAGE_LEFT) {
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
          this->dispatch(EventType::STAGE_LEFT, EventStageLeft(), getStore());
        } else {
          std::cerr << "Unkown event " << event << std::endl;
        }
      }
      catch(const std::exception& e) {
        std::cerr << "[ERROR] std::exception: " << e.what() << std::endl;
      }
      catch(...) {
        std::cerr << "[ERROR] error parsing" << std::endl;
      }
    }
  });

  wsclient_.set_close_handler([](websocket_close_status status,
                                 const utility::string_t& reason,
                                 const std::error_code& code) {
    if(int(status) == 1006) {
      // TODO: Reconnect
      std::cout << "TODO: RECONNECT" << std::endl;
    } else {
      std::cout << " closing reason..." << reason << "\n";
      std::cout << "connection closed, reason: " << reason
                << " close status: " << int(status) << " error code " << code
                << std::endl;
    }
  });

  nlohmann::json identificationJson;
  identificationJson["token"] = apiToken;
  identificationJson["device"] = initialDevice;
  this->sendAsync("token", identificationJson.dump());

  return receiveTask_;
}

void DigitalStage::Client::send(const std::string& event,
                                const std::string& message)
{
  sendAsync(event, message).wait();
}

pplx::task<void> DigitalStage::Client::sendAsync(const std::string& event,
                                                 const std::string& message)
{
  websocket_outgoing_message msg;
  std::string body_str(R"({"type":0,"data":[")" + event + "\"," + message +
                       "]}");
  msg.set_utf8_message(body_str);
#ifdef DEBUG_EVENTS
#ifdef DEBUG_PAYLOADS
  std::cout << "[SENDING] " << body_str << std::endl;
#else
  std::cout << "[SENDING] " << event << std::endl;
#endif
#endif
  return wsclient_.send(msg);
}