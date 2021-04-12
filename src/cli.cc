
#include "AuthService.h"
#include "Client.h"
#include <iostream>
#include <nlohmann/json.hpp>

using namespace DigitalStage;

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
    initialDevice["mac"] = "123456";
    initialDevice["type"] = "ov";

    Client* client = new Client("ws://localhost:4000");
    client->connect(apiToken, initialDevice);

    std::cout << "Started client" << std::endl;
  }
  catch(std::exception& err) {
    std::cerr << err.what() << std::endl;
  }

  return 0;
}
