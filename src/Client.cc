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
          }
          for(const auto& item : payload["customGroupPositions"]) {
          }
          for(const auto& item : payload["stageMembers"]) {
            store_->createStageMember(item);
            this->dispatch(EventType::STAGE_MEMBER_ADDED,
                           EventStageMemberAdded(item.get<stage_member_t>()),
                           getStore());
          }
          for(const auto& item : payload["customStageMemberVolumes"]) {
          }
          for(const auto& item : payload["customStageMemberPositions"]) {
          }
          for(const auto& item : payload["remoteAudioTracks"]) {
          }
          for(const auto& item : payload["remoteVideoTracks"]) {
          }
          for(const auto& item : payload["customRemoteAudioTrackPositions"]) {
          }
          for(const auto& item : payload["customRemoteAudioTrackVolumes"]) {
          }
          auto stageId = payload["stageId"].get<std::string>();
          auto groupId = payload["groupId"].get<std::string>();
          this->dispatch(EventType::STAGE_JOINED,
                         EventStageJoined(stageId, groupId), getStore());

          /*
           * STAGE LEFT
           */
        } else if(event == WSEvents::STAGE_LEFT) {
          // TODO: Remove all stage related data
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