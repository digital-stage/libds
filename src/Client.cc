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
        std::cout << "[EVENT] " << event << " " << payload.dump() << std::endl;
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
          store_->createSoundCard(payload);
          this->dispatch(EventType::SOUND_CARD_ADDED,
                         EventSoundCardAdded(payload.get<soundcard_t>()),
                         getStore());
        } else if(event == WSEvents::GROUP_CHANGED) {
          store_->updateSoundCard(payload);
          const std::string id = payload["_id"];
          this->dispatch(EventType::SOUND_CARD_ADDED,
                         EventSoundCardChanged(id, payload), getStore());
        } else if(event == WSEvents::GROUP_REMOVED) {
          const std::string id = payload;
          store_->removeSoundCard(id);
          this->dispatch(EventType::SOUND_CARD_REMOVED,
                         EventSoundCardRemoved(id), getStore());

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
  websocket_outgoing_message msg;
  std::string body_str(R"({"type":0,"data":[")" + event + "\"," + message +
                       "]}");
  msg.set_utf8_message(body_str);
#ifdef DEBUG_EVENTS
  std::cout << "[SENDING] " << body_str << std::endl;
#endif
  wsclient_.send(msg).wait();
}

pplx::task<void> DigitalStage::Client::sendAsync(const std::string& event,
                                                 const std::string& message)
{
  websocket_outgoing_message msg;
  std::string body_str(R"({"type":0,"data":[")" + event + "\"," + message +
                       "]}");
  msg.set_utf8_message(body_str);
#ifdef DEBUG_EVENTS
  std::cout << "[SENDING] " << body_str << std::endl;
#endif
  return wsclient_.send(msg);
}