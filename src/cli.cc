
#include "DigitalStage/Api/Client.h"
#include "DigitalStage/Auth/AuthService.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

using namespace DigitalStage::Api;
using namespace DigitalStage::Auth;

void printStage(const Store* s)
{
  auto stages = s->stages.getAll();
  for(const auto& stage : stages) {
    std::cout << "[" << stage.name << "] " << std::endl;
    auto groups = s->getGroupsByStage(stage._id);
    for(const auto& group : groups) {
      std::cout << "  [" << group.name << "]" << std::endl;
      auto stageMembers = s->getStageMembersByGroup(group._id);
      for(const auto& stageMember : stageMembers) {
        auto user = s->users.get(stageMember.userId);
        std::cout << "    [" << stageMember._id << ": "
                  << (user ? user->name : "") << "]" << std::endl;
        auto stageDevices = s->getStageDevicesByStageMember(stageMember._id);
        for(const auto& stageDevice : stageDevices) {
          auto videoTracks =
              s->getVideoTracksByStageDevice(stageDevice._id);
          for(const auto& videoTrack : videoTracks) {
            std::cout << "      [Video Track " << videoTrack._id << "]"
                      << std::endl;
          }
          auto audioTracks =
              s->getAudioTracksByStageDevice(stageDevice._id);
          for(const auto& audioTrack : audioTracks) {
            std::cout << "      [Audio Track " << audioTrack._id << "]"
                      << std::endl;
          }
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
  auto device = s->devices.get(id);
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
  auto stage = s->stages.get(stageId);
  auto group = s->groups.get(groupId);
  std::cout << "JOINED STAGE " << stage->name << " AND GROUP " << group->name
            << std::endl;
}

void handleStageDeviceChanged(const std::string& id, nlohmann::json, const Store* s)
{
  auto d = s->stageDevices.get(id);
  std::cout << "Stage device " << id << " changed" << std::endl;
}

void handleCustomStageDevicePositionChanged(const std::string& id, nlohmann::json, const Store* s)
{
  auto d = s->customStageDevicePositions.get(id);
  std::cout << "Custom stage device position " << id << " changed to (" << d->x << "|" << d->y << "|" << d->z << ") with angle " << d->rZ << "deg" << std::endl;
}

void handleCustomStageMemberVolumeChanged(const std::string& id, nlohmann::json, const Store* s)
{
  auto d = s->customStageMemberVolumes.get(id);
  std::cout << "Custom stage member volume " << id << " changed to " << d->volume << " and is " << (d->muted ? "muted" : "unmuted") << std::endl;
}

void handleStageLeft(const Store*)
{
  std::cout << "STAGE LEFT" << std::endl;
}

int main(int argc, char* argv[])
{
  if(argc != 3) {
    std::wcout << "Call this with email and password as parameters" << std::endl;
    return -1;
  }
  auto email = argv[1];
  auto password = argv[2];
  auto authService = AuthService(U("https://digitalstage-auth.germanywestcentral.cloudapp.azure.com"));

  std::cout << "Signing in..." << std::endl;
  try {
    string_t emailStr(email, email + strlen(email));
    string_t passwordStr(password, password + strlen(password));
    auto apiToken = authService.signIn(emailStr, passwordStr).get();
#ifdef WIN32
    std::wcout << "Token: " << apiToken << std::endl;
#else
    std::cout << "Token: " << apiToken << std::endl;
#endif

    nlohmann::json initialDevice;
    initialDevice["uuid"] = "123456";
    initialDevice["type"] = "ov";
    initialDevice["canAudio"] = true;
    initialDevice["canVideo"] = false;
    auto* client = new Client("wss://digitalstage-api.germanywestcentral.cloudapp.azure.com");

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
    client->stageDeviceChanged.connect(handleStageDeviceChanged);
    client->customStageDevicePositionChanged.connect(handleCustomStageDevicePositionChanged);
    client->customStageMemberVolumeChanged.connect(handleCustomStageMemberVolumeChanged);

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
    std::cerr << "Got exception: " << err.what() << std::endl;
    return -1;
  }

  while(true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
