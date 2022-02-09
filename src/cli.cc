
#include "DigitalStage/Api/Client.h"
#include "DigitalStage/Auth/AuthService.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <thread>

using namespace DigitalStage::Api;
using namespace DigitalStage::Auth;

void printStage(std::weak_ptr<DigitalStage::Api::Store> store)
{
    auto s = store.lock();
    auto stages = s->stages.getAll();
    for (const auto& stage : stages) {
        std::cout << "[" << stage.name << "] " << std::endl;
        auto groups = s->getGroupsByStage(stage._id);
        for (const auto& group : groups) {
            std::cout << "  [" << group.name << "]" << std::endl;
            auto stageMembers = s->getStageMembersByGroup(group._id);
            for (const auto& stageMember : stageMembers) {
                auto user = s->users.get(stageMember.userId);
                std::cout << "    [" << stageMember._id << ": " << (user ? user->name : "") << "]" << std::endl;
                auto stageDevices = s->getStageDevicesByStageMember(stageMember._id);
                for (const auto& stageDevice : stageDevices) {
                    auto videoTracks = s->getVideoTracksByStageDevice(stageDevice._id);
                    for (const auto& videoTrack : videoTracks) {
                        std::cout << "      [Video Track " << videoTrack._id << "]" << std::endl;
                    }
                    auto audioTracks = s->getAudioTracksByStageDevice(stageDevice._id);
                    for (const auto& audioTrack : audioTracks) {
                        std::cout << "      [Audio Track " << audioTrack._id << "]" << std::endl;
                    }
                }
            }
        }
    }
}

void handleLocalDeviceReady(const Device& d, std::weak_ptr<DigitalStage::Api::Store>)
{
    std::cout << "Local device " << d._id << " ready" << std::endl;
}

void handleDeviceAdded(const Device& d, std::weak_ptr<DigitalStage::Api::Store>)
{
    std::cout << "NEW Device " << d._id << " added" << std::endl;
}

void handleDeviceChanged(const ID_TYPE& id, const nlohmann::json& update, std::weak_ptr<DigitalStage::Api::Store> store)
{
    auto s = store.lock();
    auto device = s->devices.get(id);
    if (device) {
        std::cout << device->type << " device has been updated: " << update.dump() << std::endl;
    }
}

void handleDeviceRemoved(const ID_TYPE& id, std::weak_ptr<DigitalStage::Api::Store>)
{
    std::cout << "Device " << id << " removed" << std::endl;
}

void handleReady(std::weak_ptr<DigitalStage::Api::Store>)
{
    std::cout << "READY TO GO!" << std::endl;
}

void handleStageJoined(const ID_TYPE& stageId, std::optional<ID_TYPE> groupId, std::weak_ptr<DigitalStage::Api::Store> store)
{
    auto s = store.lock();
    auto stage = s->stages.get(stageId);
    if (groupId) {
        auto group = s->groups.get(*groupId);
        std::cout << "JOINED STAGE " << stage->name << " AND GROUP " << group->name << std::endl;
    }
    else {
        std::cout << "JOINED STAGE " << stage->name << " WITHOUT GROUP" << std::endl;
    }
}

void handleStageDeviceChanged(const std::string& id, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store> store)
{
    auto s = store.lock();
    auto d = s->stageDevices.get(id);
    std::cout << "Stage device " << id << " changed" << std::endl;
}

void handleStageLeft(std::weak_ptr<DigitalStage::Api::Store>)
{
    std::cout << "STAGE LEFT" << std::endl;
}


void attachHandlers(Client* client)
{
    client->ready.connect([](std::weak_ptr<DigitalStage::Api::Store>) { std::cout << "Ready - this type inside an anonymous callback function" << std::endl; });

    client->ready.connect(handleReady);

    client->deviceAdded.connect(handleDeviceAdded);
    client->deviceChanged.connect(handleDeviceChanged);
    client->deviceRemoved.connect(handleDeviceRemoved);
    client->localDeviceReady.connect(handleLocalDeviceReady);
    client->stageJoined.connect(handleStageJoined);
    client->stageLeft.connect(handleStageLeft);
    client->stageDeviceChanged.connect(handleStageDeviceChanged);

    // Always print on stage changes
    client->stageJoined.connect([](const auto&, const auto&, std::weak_ptr<DigitalStage::Api::Store> s) { printStage(s); });
    client->stageLeft.connect([](std::weak_ptr<DigitalStage::Api::Store> s) { printStage(s); });
    client->groupAdded.connect([](const auto&, std::weak_ptr<DigitalStage::Api::Store> s) { printStage(s); });
    client->groupChanged.connect([](const auto&, const auto&, std::weak_ptr<DigitalStage::Api::Store> s) { printStage(s); });
    client->groupRemoved.connect([](const auto&, std::weak_ptr<DigitalStage::Api::Store> s) { printStage(s); });
    client->stageMemberAdded.connect([](const auto&, std::weak_ptr<DigitalStage::Api::Store> s) { printStage(s); });
    client->stageMemberChanged.connect([](const auto&, const auto&, std::weak_ptr<DigitalStage::Api::Store> s) { printStage(s); });
    client->stageMemberRemoved.connect([](const auto&, std::weak_ptr<DigitalStage::Api::Store> s) { printStage(s); });
}


int main(int argc, char* argv[])
{
    auto email = "test@digital-stage.org";
    auto password = "test123test123test!";
    if (argc == 3) {
        email = argv[1];
        password = argv[2];
    }
    auto authService = AuthService("https://auth.dstage.org");

    std::cout << "Signing in..." << std::endl;
    try {
        std::string emailStr(email, email + strlen(email));
        std::string passwordStr(password, password + strlen(password));
        auto apiToken = authService.signInSync(emailStr, passwordStr);
        std::cout << "Token: " << apiToken << std::endl;

        nlohmann::json initialDevice;
        initialDevice["uuid"] = "123456";
        initialDevice["type"] = "ov";
        initialDevice["canAudio"] = true;
        initialDevice["canVideo"] = false;

        std::unique_ptr<Client> client = std::make_unique<Client>("wss://api.dstage.org");
        attachHandlers(client.get());
        client->connect(apiToken, initialDevice);
        std::cout << "Started client" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // Disconnect and connect using same client
        client->disconnect();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        client->connect(apiToken, initialDevice);
        std::cout << "Manually reconnected client" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // Connect using another client
        client = std::make_unique<Client>("wss://api.dstage.org");
        attachHandlers(client.get());
        std::cout << "Recreated client without disconnecting before" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    catch (std::exception& err) {
        std::cerr << "Got exception: " << err.what() << std::endl;
        return -1;
    }

    return 0;
}
