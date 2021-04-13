
#include "AuthService.h"
#include "Client.h"
#include "eventpp/utilities/argumentadapter.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

using namespace DigitalStage;

void handleLocalDeviceReady(const EventLocalDeviceReady& e, const Store& s)
{
  std::cout << "Local device " << e.getDevice()._id << " ready";
}

void handleDeviceAdded(const EventDeviceAdded& e, const Store& s)
{
  std::cout << "Device " << e.getDevice()._id << " added";
}
void handleDeviceChanged(const EventDeviceChanged& e, const Store& s)
{
  std::cout << "Device " << e.getId() << " changed";
  // std::cout << "UUID of device is " << s.getDevices()[e.getDeviceId()].uuid
  // <<
}
void handleDeviceRemoved(const EventDeviceAdded& e)
{
  std::cout << "Device " << e.getDevice()._id << " removed";
}

int main(int argc, char const* argv[])
{
  /* code */
  std::cout << "Hello World!" << std::endl;

  auto authService = AuthService("https://single.dstage.org/api/auth");

  std::cout << "Signing in..." << std::endl;
  try {
    auto apiToken =
        authService.signIn("tobias.hegemann@me.com", "Testtesttest123!").get();
    std::cout << "Token: " << apiToken << std::endl;

    nlohmann::json initialDevice;
    initialDevice["uuid"] = "123456";
    initialDevice["type"] = "ov";
    initialDevice["canAudio"] = true;
    initialDevice["canVideo"] = false;

    Client* client = new Client("ws://localhost:4000");

    client->appendListener(EventType::READY, [](const DigitalStage::Event& e,
                                                const DigitalStage::Store& s) {
      std::cout << "READY, it works!!!" << std::endl;
    });

    client->appendListener(EventType::DEVICE_ADDED,
                           eventpp::argumentAdapter(handleDeviceAdded));

    client->appendListener(EventType::LOCAL_DEVICE_READY,
                           eventpp::argumentAdapter(handleLocalDeviceReady));

    client->connect(apiToken, initialDevice);

    std::cout << "Started client" << std::endl;
  }
  catch(std::exception& err) {
    std::cerr << err.what() << std::endl;
  }

  while(true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
