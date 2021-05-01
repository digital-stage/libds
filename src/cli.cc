
#include "DigitalStage/Api/Client.h"
#include "DigitalStage/Auth/AuthService.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

using namespace DigitalStage::Api;
using namespace DigitalStage::Auth;

void printStage(const Store* s)
{
  auto stages = s->getStages();
  for(const auto& stage : stages) {
    std::cout << "[" << stage.name << "] " << std::endl;
    auto groups = s->getGroupsByStage(stage._id);
    for(const auto& group : groups) {
      std::cout << "  [" << group.name << "]" << std::endl;
      auto stageMembers = s->getStageMembersByGroup(group._id);
      for(const auto& stageMember : stageMembers) {
        auto user = s->getUser(stageMember.userId);
        std::cout << "    [" << stageMember._id << ": "
                  << (user ? user->name : "") << "]" << std::endl;
        auto remoteVideoTracks =
            s->getRemoteVideoTracksByStageMember(stageMember._id);
        for(const auto& remoteVideoTrack : remoteVideoTracks) {
          std::cout << "      [Video Track " << remoteVideoTrack._id << "]"
                    << std::endl;
        }
        auto remoteAudioTracks =
            s->getRemoteAudioTracksByStageMember(stageMember._id);
        for(const auto& remoteAudioTrack : remoteAudioTracks) {
          std::cout << "      [Audio Track " << remoteAudioTrack._id << "]"
                    << std::endl;
        }
      }
    }
  }
}

void handleLocalDeviceReady(const Device& d, const Store*)
{
  std::cout << "Local device " << d._id << " ready" << std::endl;
}

void handleDeviceAdded(const Device& d, const Store*)
{
  std::cout << "NEW Device " << d._id << " added" << std::endl;
}
void handleDeviceChanged(const ID_TYPE& id, const nlohmann::json& update,
                         const Store* s)
{
  auto device = s->getDevice(id);
  if(device) {
    std::cout << device->type << " device has been updated: " << update.dump()
              << std::endl;
  }
}
void handleDeviceRemoved(const ID_TYPE& id, const Store*)
{
  std::cout << "Device " << id << " removed" << std::endl;
}
void handleReady(const Store*)
{
  std::cout << "READY TO GO!" << std::endl;
}
void handleStageJoined(const ID_TYPE& stageId, const ID_TYPE& groupId,
                       const Store* s)
{
  auto stage = s->getStage(stageId);
  auto group = s->getGroup(groupId);
  std::cout << "JOINED STAGE " << stage->name << " AND GROUP " << group->name
            << std::endl;
}

void handleStageLeft(const Store*)
{
  std::cout << "STAGE LEFT" << std::endl;
}

int main(int, char const*[])
{
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
    initialDevice["availableSoundCardIds"] = {};
    auto* client = new Client("ws://localhost:4000");

    client->ready.connect([](const Store*) {
      std::cout << "Ready - this type inside an anonymous callback function"
                << std::endl;
    });

    client->ready.connect(handleReady);

    client->deviceAdded.connect(handleDeviceAdded);
    client->deviceChanged.connect(handleDeviceChanged);
    client->deviceRemoved.connect(handleDeviceRemoved);
    client->localDeviceReady.connect(handleLocalDeviceReady);
    client->stageJoined.connect(handleStageJoined);
    client->stageLeft.connect(handleStageLeft);

    // Always print on stage changes
    client->stageJoined.connect(
        [](const auto&, const auto&, const Store* s) { printStage(s); });
    client->stageLeft.connect([](const Store* s) { printStage(s); });
    client->groupAdded.connect(
        [](const auto&, const Store* s) { printStage(s); });
    client->groupChanged.connect(
        [](const auto&, const auto&, const Store* s) { printStage(s); });
    client->groupRemoved.connect(
        [](const auto&, const Store* s) { printStage(s); });
    client->stageMemberAdded.connect(
        [](const auto&, const Store* s) { printStage(s); });
    client->stageMemberChanged.connect(
        [](const auto&, const auto&, const Store* s) { printStage(s); });
    client->stageMemberRemoved.connect(
        [](const auto&, const Store* s) { printStage(s); });

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
