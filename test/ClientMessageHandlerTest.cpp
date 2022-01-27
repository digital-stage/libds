//
// Created by Tobias Hegemann on 27.01.22.
//

#include <gtest/gtest.h>

#include <thread>

#include <DigitalStage/Api/Client.h>

TEST(ClientTest, MessageHandler) {
  auto client = std::make_shared<DigitalStage::Api::Client>(API_URL, false);

  // We don't connect, instead validating the handleMessage method of the client
  EXPECT_NO_THROW(client->handleMessage("s-a", {{"_id", "1234"}, {"name", "Teststage"}}));

  EXPECT_THROW(client->handleMessage(DigitalStage::Api::RetrieveEvents::LOCAL_DEVICE_READY, {}), DigitalStage::Api::InvalidPayloadException);
  EXPECT_THROW(client->handleMessage(DigitalStage::Api::RetrieveEvents::LOCAL_DEVICE_READY, {{"none", "1234"}}), DigitalStage::Api::InvalidPayloadException);
}