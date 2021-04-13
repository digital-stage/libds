
#include "AuthService.h"
#include "Client.h"
#include "eventpp/utilities/argumentadapter.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

using namespace DigitalStage;

void handleLocalDeviceReady(const EventLocalDeviceReady& e, const Store& s)
{
  std::cout << "Local device " << e.getDevice()._id << " ready" << std::endl;
}

void handleDeviceAdded(const EventDeviceAdded& e, const Store& s)
{
  std::cout << "Device " << e.getDevice()._id << " added" << std::endl;
}
void handleDeviceChanged(const EventDeviceChanged& e, const Store& s)
{
  std::cout << "Device " << e.getId() << " changed" << std::endl;
  // std::cout << "UUID of device is " << s.getDevices()[e.getDeviceId()].uuid
  auto device = s.getDevice(e.getId());
  if(device) {
    std::cout << "Device MAC is " << device->uuid << std::endl;
  }
}
void handleDeviceRemoved(const EventDeviceAdded& e, const Store& s)
{
  std::cout << "Device " << e.getDevice()._id << " removed" << std::endl;
}
void handleReady(const EventReady& e, const Store& s)
{
  std::cout << "READY TO GO!" << std::endl;
  std::cout << "Stages:" << std::endl;
  auto stages = s.getStages();
  for(const auto& stage : stages) {
    std::cout << "[" << stage.name << "] " << std::endl;
    auto groups = s.getGroupsByStage(stage._id);
    for(const auto& group : groups) {
      std::cout << "  [" << group.name << "]" << std::endl;

      auto stageMembers = s.getStageMembersByGroup(group._id);
      for(const auto& stageMember : stageMembers) {
        auto user = s.getUser(stageMember.userId);
        std::cout << "    [" << stageMember._id << ": "
                  << (user ? user->name : "") << "]" << std::endl;
      }
    }
  }
}
void handleStageJoined(const EventStageJoined& e, const Store& s)
{
  auto stage = s.getStage(e.getStageId());
  auto group = s.getGroup(e.getGroupId());
  std::cout << "JOINED STAGE " << stage->name << " AND GROUP " << group->name
            << std::endl;
  std::cout << "Stages:" << std::endl;
  auto stages = s.getStages();
  for(const auto& stage : stages) {
    std::cout << "[" << stage.name << "] " << std::endl;
    auto groups = s.getGroupsByStage(stage._id);
    for(const auto& group : groups) {
      std::cout << "  [" << group.name << "]" << std::endl;

      auto stageMembers = s.getStageMembersByGroup(group._id);
      for(const auto& stageMember : stageMembers) {
        auto user = s.getUser(stageMember.userId);
        std::cout << "    [" << stageMember._id << ": "
                  << (user ? user->name : "") << "]" << std::endl;
      }
    }
  }
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

    client->appendListener(EventType::READY,
                           eventpp::argumentAdapter(handleReady));

    client->appendListener(EventType::DEVICE_ADDED,
                           eventpp::argumentAdapter(handleDeviceAdded));

    client->appendListener(EventType::LOCAL_DEVICE_READY,
                           eventpp::argumentAdapter(handleLocalDeviceReady));

    client->appendListener(EventType::STAGE_JOINED,
                           eventpp::argumentAdapter(handleStageJoined));

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
