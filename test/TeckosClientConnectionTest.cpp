#include <gtest/gtest.h>

#include <thread>
#include <DigitalStage/Auth/AuthService.h>d
#include <teckos/client.h>

TEST(TeckosClientTest, Connection) {
  // Get token
  auto auth = std::make_shared<DigitalStage::Auth::AuthService>(AUTH_URL);
  std::string token;
  EXPECT_NO_THROW(token = auth->signInSync("test@digital-stage.org", "test123test123test!"));

  nlohmann::json initialDevice;
  initialDevice["uuid"] = "123456";
  initialDevice["type"] = "ov";
  initialDevice["canAudio"] = false;
  initialDevice["canVideo"] = false;

  auto client = std::make_shared<teckos::client>();

  /* WITH TIMEOUTS */
  std::cout << "Connecting" << std::endl;
  EXPECT_NO_THROW(client->connect(API_URL, token, initialDevice));

  std::cout << "Waiting for 1 second" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "Disconnecting" << std::endl;
  EXPECT_NO_THROW(client->disconnect());

  std::cout << "Connecting" << std::endl;
  EXPECT_NO_THROW(client->connect(API_URL, token, initialDevice));

  std::cout << "Waiting for 1 second" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "Replace client object" << std::endl;
  client = std::make_shared<teckos::client>();

  // And connecting again
  std::cout << "Connecting" << std::endl;
  EXPECT_NO_THROW(client->connect(API_URL, token, initialDevice));

  std::cout << "Waiting for 1 second" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "Disconnecting" << std::endl;
  EXPECT_NO_THROW(client->disconnect());


  /* WITHOUT TIMEOUTS */
  std::cout << std::endl << "Now without timeouts" << std::endl;
  std::cout << "Connecting" << std::endl;
  EXPECT_NO_THROW(client->connect(API_URL, token, initialDevice));

  std::cout << "Disconnecting" << std::endl;
  EXPECT_NO_THROW(client->disconnect());

  std::cout << "Connecting" << std::endl;
  EXPECT_NO_THROW(client->connect(API_URL, token, initialDevice));

  std::cout << "Replace client object" << std::endl;
  client = std::make_shared<teckos::client>();

  std::cout << "Connecting" << std::endl;
  EXPECT_NO_THROW(client->connect(API_URL, token, initialDevice));

  // Disconnecting
  std::cout << "Disconnecting" << std::endl;
  EXPECT_NO_THROW(client->disconnect());
}
