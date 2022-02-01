#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <DigitalStage/Api/Client.h>
#include <DigitalStage/Auth/AuthService.h>
#include <DigitalStage/Types.h>

TEST(ClientTest, AsyncLive) {
  // Get token
  auto auth = std::make_shared<DigitalStage::Auth::AuthService>(AUTH_URL);
  std::string token;
  EXPECT_NO_THROW(token = auth->signInSync("test@digital-stage.org", "test123test123test!"));
  auto client = std::make_shared<DigitalStage::Api::Client>(API_URL, true);
  std::cout << "Got token: " << token << std::endl;

  client->error.connect([](const std::exception &e){
    std::cerr << e.what() << std::endl;
    FAIL();
  });

  // Process ready
  client->ready.connect([=](const DigitalStage::Api::Store *store) {
    // Never send and aspect another callback inside the callback, so use a different thread here
    std::cout << "[CONNECTED]" << std::endl;
    EXPECT_TRUE(store->isReady());
    EXPECT_EQ(store->getLocalDevice()->uuid, "123456");

    // Create stage
    nlohmann::json createStagePayload = {
        {"name", "Testbühne"},
    };

    std::cout << "Create stage" << std::endl;
    client->send(DigitalStage::Api::SendEvents::CREATE_STAGE,
                 {{"name", "Testbühne"}, {"videoType", "web"}, {"audioType", "jammer"}},
                 [](teckos::Result result) {
                   EXPECT_TRUE(result.at(0).is_null());
                   std::cout << "Created stage" << std::endl;
                 });
    // Expect the stage created asynchronously (not waiting for callback thread)
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto stages = store->stages.getAll();
    EXPECT_GE(stages.size(), 1);
    auto stageIter = std::find_if(stages.begin(), stages.end(), [&](const auto &stage) {
      return stage.name == "Testbühne";
    });
    if (stageIter == std::end(stages)) {
      FAIL();
    }
    auto stage = *stageIter;
    EXPECT_EQ(stage.name, "Testbühne");

    std::cout << "Create group for stage " << stage._id << std::endl;
    teckos::Callback callback;
    client->send(DigitalStage::Api::SendEvents::CREATE_GROUP,
                 {{"stageId", stage._id}, {"name", "Testgruppe"}},
                 [](teckos::Result result) {
                   EXPECT_TRUE(result.at(0).is_null());
                   std::cout << "Created group" << std::endl;
                 });
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto groups = store->getGroupsByStage(stage._id);
    EXPECT_GE(groups.size(), 1);
    auto groupsIter = std::find_if(groups.begin(), groups.end(), [&](const auto &group) {
      return group.name == "Testgruppe";
    });
    EXPECT_NE(groupsIter, std::end(groups));
    if (groupsIter != std::end(groups)) {

      auto group = *groupsIter;
      EXPECT_EQ(group.name, "Testgruppe");

      std::cout << "Generate invalid invitation codes using a non exising stage" << std::endl;
      EXPECT_ANY_THROW(client->encodeInvitationCode("invalid").get());
      EXPECT_ANY_THROW(client->encodeInvitationCode("invalid", "invalid").get());

      std::cout << "Decode invalid invitation codes" << std::endl;
      EXPECT_ANY_THROW(client->decodeInvitationCode("invalid").get());
      EXPECT_ANY_THROW(client->decodeInvitationCode("").get());
      EXPECT_ANY_THROW(client->decodeInvitationCode("123").get());
      EXPECT_ANY_THROW(client->decodeInvitationCode("ABC").get());

      std::cout << "Generate invite code with group" << std::endl;
      auto invite_code_with_group = client->encodeInvitationCode(stage._id, group._id).get();

      std::cout << "Decode invite code with group" << std::endl;
      auto invite_pair = client->decodeInvitationCode(invite_code_with_group).get();
      EXPECT_EQ(invite_pair.first, stage._id);
      EXPECT_EQ(invite_pair.second, group._id);

      std::cout << "Generate invite code without group" << std::endl;
      auto invite_code_without_group = client->encodeInvitationCode(stage._id).get();

      std::cout << "Decode invite code without group" << std::endl;
      auto invite_pair_without_group = client->decodeInvitationCode(invite_code_without_group).get();
      EXPECT_EQ(invite_pair_without_group.first, stage._id);
      std::cout << *invite_pair_without_group.second << std::endl;
      if(invite_pair_without_group.second) {
        FAIL() << "GroupId of decoded code (created without group) is not std::nullopt, but: " << *invite_pair_without_group.second;
      }

      std::cout << "Join stage and group" << std::endl;
      client->send(DigitalStage::Api::SendEvents::JOIN_STAGE,
                   {{"stageId", stage._id}, {"groupId", group._id}});
      std::this_thread::sleep_for(std::chrono::seconds(1));
      EXPECT_EQ(store->getStageId(), stage._id);
      EXPECT_EQ(store->getGroupId(), group._id);

      std::cout << "Leave stage" << std::endl;
      client->send(DigitalStage::Api::SendEvents::LEAVE_STAGE,
                   {});
      std::this_thread::sleep_for(std::chrono::seconds(1));
      EXPECT_NE(store->getStageId(), stage._id);
      EXPECT_NE(store->getGroupId(), group._id);

      std::cout << "Join stage and group (again)" << std::endl;
      client->send(DigitalStage::Api::SendEvents::JOIN_STAGE,
                   {{"stageId", stage._id}, {"groupId", group._id}});
      std::this_thread::sleep_for(std::chrono::seconds(1));
      EXPECT_EQ(store->getStageId(), stage._id);
      EXPECT_EQ(store->getGroupId(), group._id);

      std::cout << "Join stage without specifying group" << std::endl;
      client->send(DigitalStage::Api::SendEvents::JOIN_STAGE,
                   {{"stageId", stage._id}});
      std::this_thread::sleep_for(std::chrono::seconds(1));
      EXPECT_EQ(store->getStageId(), stage._id);
      EXPECT_EQ(store->getGroupId(), group._id);

      std::cout << "Join stage without group (using null)" << std::endl;
      client->send(DigitalStage::Api::SendEvents::JOIN_STAGE,
                   {{"stageId", stage._id}, {"groupId", nullptr}});
      std::this_thread::sleep_for(std::chrono::seconds(1));
      EXPECT_EQ(store->getStageId(), stage._id);
      if(store->getGroupId()) {
        FAIL() << "GroupId is not std::nullopt, but: " << *store->getGroupId();
      }
    }

    // Remove all stages
    std::cout << "Cleaning up: removing all stages" << std::endl;
    stages = store->stages.getAll();
    for (const auto &item: stages) {
      client->send(DigitalStage::Api::SendEvents::REMOVE_STAGE, item._id);
    }
    std::this_thread::sleep_for(std::chrono::seconds(4));
    EXPECT_EQ(store->stages.getAll().size(), 0);

    // Expect to be outside any stage
    EXPECT_NE(store->getStageId(), stage._id);
  });

  nlohmann::json initialDevice;
  initialDevice["uuid"] = "123456";
  initialDevice["type"] = "ov";
  initialDevice["canAudio"] = false;
  initialDevice["canVideo"] = false;
  std::cout << "Connecting with token " << token << " ...   ";
  EXPECT_NO_THROW(client->connect(token, initialDevice));

  std::this_thread::sleep_for(std::chrono::seconds(8));
  std::cout << "Closing connection...   ";
  EXPECT_NO_THROW(client->disconnect());
  std::cout << "[CLOSED]" << std::endl;

  std::cout << "Closing connection again...   ";
  EXPECT_NO_THROW(client->disconnect());
  std::cout << "[OK]" << std::endl;

  std::cout << "Replace client and connect...";
  client = std::make_shared<DigitalStage::Api::Client>(API_URL);
  EXPECT_NO_THROW(client->connect(token, initialDevice));

  std::cout << "Replace client without disconnecting...";
  EXPECT_NO_THROW(client.reset());

  std::cout << "Signing out...   ";
  auth->signOutSync(token);
  std::cout << "[SIGNED OUT]" << std::endl;
}