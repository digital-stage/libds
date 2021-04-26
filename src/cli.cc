
#include "AuthService.h"
#include "Client.h"
#include "eventpp/utilities/argumentadapter.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

using namespace DigitalStage;

void printStage(const Store& s)
{
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
        auto remoteVideoTracks =
            s.getRemoteVideoTracksByStageMember(stageMember._id);
        for(const auto& remoteVideoTrack : remoteVideoTracks) {
          std::cout << "      [Video Track " << remoteVideoTrack._id << "]"
                    << std::endl;
        }
        auto remoteAudioTracks =
            s.getRemoteAudioTracksByStageMember(stageMember._id);
        for(const auto& remoteAudioTrack : remoteAudioTracks) {
          std::cout << "      [Audio Track " << remoteAudioTrack._id << "]"
                    << std::endl;
        }
      }
    }
  }
}

void handleLocalDeviceReady(const EventLocalDeviceReady& e, const Store&)
{
  std::cout << "Local device " << e.getDevice()._id << " ready" << std::endl;
}

void handleDeviceAdded(const EventDeviceAdded& e, const Store&)
{
  std::cout << "NEW Device " << e.getDevice()._id << " added" << std::endl;
}
void handleDeviceChanged(const EventDeviceChanged& e, const Store& s)
{
  auto device = s.getDevice(e.getId());
  if(device) {
    std::cout << device->type
              << " device has been updated: " << e.getUpdate().dump()
              << std::endl;
  }
}
void handleDeviceRemoved(const EventDeviceRemoved& e, const Store&)
{
  std::cout << "Device " << e.getId() << " removed" << std::endl;
}
void handleReady(const EventReady&, const Store&)
{
  std::cout << "READY TO GO!" << std::endl;
}
void handleStageJoined(const EventStageJoined& e, const Store& s)
{
  auto stage = s.getStage(e.getStageId());
  auto group = s.getGroup(e.getGroupId());
  std::cout << "JOINED STAGE " << stage->name << " AND GROUP " << group->name
            << std::endl;
}

void handleStageLeft(const EventStageLeft&, const Store&)
{
  std::cout << "STAGE LEFT" << std::endl;
}

void handleStageChanges(const Event&, const Store& s)
{
  printStage(s);
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

    Client* client = new Client("ws://localhost:4000");

    client->appendListener(EventType::READY, [](const DigitalStage::Event&,
                                                const DigitalStage::Store&) {
      std::cout << "Ready - this type inside an anonymous callback function"
                << std::endl;
    });

    client->appendListener(EventType::READY,
                           eventpp::argumentAdapter(handleReady));

    client->appendListener(EventType::DEVICE_ADDED,
                           eventpp::argumentAdapter(handleDeviceAdded));

    client->appendListener(EventType::DEVICE_CHANGED,
                           eventpp::argumentAdapter(handleDeviceChanged));

    client->appendListener(EventType::DEVICE_REMOVED,
                           eventpp::argumentAdapter(handleDeviceRemoved));

    client->appendListener(EventType::LOCAL_DEVICE_READY,
                           eventpp::argumentAdapter(handleLocalDeviceReady));

    client->appendListener(EventType::STAGE_JOINED,
                           eventpp::argumentAdapter(handleStageJoined));

    client->appendListener(EventType::STAGE_JOINED,
                           eventpp::argumentAdapter(handleStageLeft));

    // Always print on stage changes
    client->appendListener(EventType::STAGE_JOINED,
                           eventpp::argumentAdapter(handleStageChanges));
    client->appendListener(EventType::GROUP_REMOVED,
                           eventpp::argumentAdapter(handleStageChanges));
    client->appendListener(EventType::GROUP_ADDED,
                           eventpp::argumentAdapter(handleStageChanges));
    client->appendListener(EventType::GROUP_REMOVED,
                           eventpp::argumentAdapter(handleStageChanges));
    client->appendListener(EventType::REMOTE_VIDEO_TRACK_ADDED,
                           eventpp::argumentAdapter(handleStageChanges));
    client->appendListener(EventType::REMOTE_VIDEO_TRACK_REMOVED,
                           eventpp::argumentAdapter(handleStageChanges));
    client->appendListener(EventType::REMOTE_AUDIO_TRACK_ADDED,
                           eventpp::argumentAdapter(handleStageChanges));
    client->appendListener(EventType::REMOTE_AUDIO_TRACK_REMOVED,
                           eventpp::argumentAdapter(handleStageChanges));
    client->appendListener(EventType::STAGE_MEMBER_ADDED,
                           eventpp::argumentAdapter(handleStageChanges));
    client->appendListener(EventType::STAGE_MEMBER_REMOVED,
                           eventpp::argumentAdapter(handleStageChanges));

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
