#ifndef DS_CLIENT
#define DS_CLIENT

#include "Events.h"
#include "Store.h"
#include <cpprest/ws_client.h>
#include <eventpp/eventdispatcher.h>
#include <mutex>
#include <pplx/pplxtasks.h>

using namespace web::websockets::client;

namespace DigitalStage {
  namespace Api {
    class Client
        : public eventpp::EventDispatcher<EventType,
                                          void(const Event&,
                                               const Store&)> {
    public:
      Client(const std::string& apiUrl);

      pplx::task<void>
      connect(const std::string& apiToken,
              const nlohmann::json initialDevicePayload = nullptr);

      void disconnect();

      const Store& getStore();

      bool isConnected();

    protected:
      void send(const std::string& event, const std::string& message);

      pplx::task<void> sendAsync(const std::string& event,
                                 const std::string& message);

    private:
      const std::string apiUrl_;
      std::unique_ptr<Store> store_;

      pplx::task<void> receiveTask_;
      websocket_callback_client wsclient_;
    };
  } // namespace Api
} // namespace DigitalStage

#endif // DS_CLIENT