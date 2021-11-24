#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <DigitalStage/Api/Client.h>
#include <DigitalStage/Auth/AuthService.h>
#include <DigitalStage/Types.h>

TEST(ClientTest, StageWorkflow)
{
  // Get token
  auto auth = std::make_shared<DigitalStage::Auth::AuthService>(AUTH_URL);
  const auto token = auth->signInSync("test@digital-stage.org", "test123test123test!");
  auto client = std::make_shared<DigitalStage::Api::Client>(API_URL);
  EXPECT_TRUE(token);

  // Process ready
  client->ready.connect([=](const DigitalStage::Api::Store* store) {
    std::cout << "[CONNECTED]" << std::endl;
    EXPECT_TRUE(store->isReady());
    EXPECT_EQ(store->getLocalDevice()->uuid, "123456");

    // Create stage
    nlohmann::json createStagePayload = {
        {"name", "Testbühne"},
    };

    std::cout << "Create stage" << std::endl;
    client->send(DigitalStage::Api::SendEvents::CREATE_STAGE, {{"name", "Testbühne"}, {"videoType", "web"}, {"audioType", "jammer"}}, [](const std::vector<nlohmann::json>& result) {
      EXPECT_TRUE(result.at(0).is_null());
      std::cout << "Created stage" << std::endl;
    });
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto stages = store->stages.getAll();
    EXPECT_GE(stages.size(), 1);
    auto stageIter = std::find_if(stages.begin(), stages.end(), [&](const auto& stage) {
      return stage.name == "Testbühne";
    });
    EXPECT_NE(stageIter, std::end(stages));
    auto stage = *stageIter;
    EXPECT_EQ(stage.name, "Testbühne");

    std::cout << "Create group for stage " << stage._id << std::endl;
    client->send(DigitalStage::Api::SendEvents::CREATE_GROUP, {{"stageId", stage._id}, {"name", "Testgruppe"}}, [](const std::vector<nlohmann::json>& result) {
      EXPECT_TRUE(result.at(0).is_null());
      std::cout << "Created group" << std::endl;
    });
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto groups = store->getGroupsByStage(stage._id);
    EXPECT_GE(groups.size(), 1);
    auto groupsIter = std::find_if(groups.begin(), groups.end(), [&](const auto& group) {
      return group.name == "Testgruppe";
    });
    EXPECT_NE(groupsIter, std::end(groups));
    auto group = *groupsIter;
    EXPECT_EQ(group.name, "Testgruppe");

    // Generate invite code
    auto invite_code = client->encodeInvitationCode(stage._id, group._id).get();

    // Use invite code
    auto invite_pair = client->decodeInvitationCode(invite_code).get();
    EXPECT_EQ(invite_pair.first, stage._id);
    EXPECT_EQ(invite_pair.second, group._id);

    // Now join stage
    std::cout << "Join stage" << std::endl;
    client->send(DigitalStage::Api::SendEvents::JOIN_STAGE, {{"stageId", stage._id}, {"groupId", group._id}}, [](const std::vector<nlohmann::json>& result) { EXPECT_TRUE(result.at(0).is_null()); });
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(store->getStageId(), stage._id);
    EXPECT_EQ(store->getGroupId(), group._id);

    // Leave stage
    std::cout << "Leave stage" << std::endl;
    client->send(DigitalStage::Api::SendEvents::LEAVE_STAGE, {}, [](const std::vector<nlohmann::json>& result) { EXPECT_TRUE(result.at(0).is_null()); });
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_NE(store->getStageId(), stage._id);
    EXPECT_NE(store->getGroupId(), group._id);

    // Join again
    std::cout << "Join stage (again)" << std::endl;
    client->send(DigitalStage::Api::SendEvents::JOIN_STAGE, {{"stageId", stage._id}, {"groupId", group._id}}, [](const std::vector<nlohmann::json>& result) { EXPECT_TRUE(result.at(0).is_null()); });
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(store->getStageId(), stage._id);
    EXPECT_EQ(store->getGroupId(), group._id);

    // Remove all stages
    std::cout << "Cleaning up: removing all stages" << std::endl;
    stages = store->stages.getAll();
    for(const auto& item : stages) {
      client->send(DigitalStage::Api::SendEvents::REMOVE_STAGE, item._id);
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_EQ(store->stages.getAll().size(), 0);

    // Expect to be outside any stage
    EXPECT_NE(store->getStageId(), stage._id);
    EXPECT_NE(store->getGroupId(), group._id);
  });

  nlohmann::json initialDevice;
  initialDevice["uuid"] = "123456";
  initialDevice["type"] = "ov";
  initialDevice["canAudio"] = false;
  initialDevice["canVideo"] = false;
  std::cout << "Connecting...   ";
  EXPECT_NO_THROW(client->connect(*token, initialDevice));

  std::this_thread::sleep_for(std::chrono::seconds(10));
  std::cout << "Closing connection...   ";
  EXPECT_NO_THROW(client->disconnect());
  std::cout << "[CLOSED]" << std::endl;
}