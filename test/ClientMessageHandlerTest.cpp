//
// Created by Tobias Hegemann on 27.01.22.
//

#include <gtest/gtest.h>

#include <thread>

#include <DigitalStage/Api/Client.h>

TEST(ClientTest, MessageHandler) {
  auto client = std::make_shared<DigitalStage::Api::Client>(API_URL, false);

  // We don't connect, instead validating the handleMessage method of the client
  EXPECT_NO_THROW(client->handleMessage(DigitalStage::Api::RetrieveEvents::LOCAL_DEVICE_READY,
                                     {
                                         {"_id", "1234"},
                                         {"userId", "1234"},
                                         {"uuid", "Teststage"},
                                         {"type", "Teststage"},
                                         {"online", true},
                                         {"canVideo", true},
                                         {"canAudio", true},
                                         {"sendVideo", true},
                                         {"sendAudio", true},
                                         {"receiveVideo", true},
                                         {"receiveAudio", true},
                                         {"buffer", 5},
                                         {"volume", 5},
                                         {"volume", 1.0F},
                                         {"balance", 1.0F},
                                     }));

  EXPECT_THROW(client->handleMessage(DigitalStage::Api::RetrieveEvents::LOCAL_DEVICE_READY,
                                     {{"_id", nullptr}, {"name", "Teststage"}}),
               DigitalStage::Api::InvalidPayloadException);

  EXPECT_THROW(client->handleMessage(DigitalStage::Api::RetrieveEvents::LOCAL_DEVICE_READY,
                                     {{"_id", nullptr}, {"name", "Teststage"}}),
               nlohmann::json::out_of_range);

  EXPECT_THROW(client->handleMessage(DigitalStage::Api::RetrieveEvents::LOCAL_DEVICE_READY,
                                     {{"_id", 1234}, {"name", "Teststage"}}),
               DigitalStage::Api::InvalidPayloadException);

  EXPECT_THROW(client->handleMessage(DigitalStage::Api::RetrieveEvents::LOCAL_DEVICE_READY, {}),
               DigitalStage::Api::InvalidPayloadException);
  EXPECT_THROW(client->handleMessage(DigitalStage::Api::RetrieveEvents::LOCAL_DEVICE_READY, {{"none", "1234"}}),
               DigitalStage::Api::InvalidPayloadException);
}