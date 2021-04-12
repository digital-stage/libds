#ifndef DS_CLIENT
#define DS_CLIENT

#include "Store.h"
#include <cpprest/ws_client.h>
#include <pplx/pplxtasks.h>
#include <thread>

using namespace web::websockets::client;

namespace DigitalStage {
  class Client {
  public:
    Client(const std::string& apiUrl);

    void connect(const std::string& apiToken,
                 nlohmann::json initialDevicePayload = nullptr);

    void disconnect();

    const Store& getStore();

  protected:
    void send(const std::string& event, const std::string& message);

    pplx::task<void> sendAsync(const std::string& event,
                               const std::string& message);

  private:
    const std::string& apiUrl_;
    std::unique_ptr<Store> store_;

    websocket_callback_client wsclient_;
  };
} // namespace DigitalStage

#endif // DS_CLIENT