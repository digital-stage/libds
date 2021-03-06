//
// Created by Tobias Hegemann on 24.02.21.
//
#include "DigitalStage/Auth/AuthService.h"
#include <teckos/rest.h>
#include <nlohmann/json.hpp>

#include "spdlog/spdlog.h"

using namespace DigitalStage::Auth;

AuthService::AuthService(const std::string &authUrl) : url_(authUrl) {
}

std::future<bool> AuthService::verifyToken(const std::string& token)
{
  return std::async(std::launch::async, [this, token] {
    return verifyTokenSync(token);
  });
}

[[maybe_unused]] bool AuthService::verifyTokenSync(const std::string& token)
{
  auto header = teckos::Header();
  header.insert({"Authorization", "Bearer " + token});
  auto result = teckos::rest::Get(this->url_ + "/profile", header);
  if(result.statusCode == 200) {
    return true;
  }
  spdlog::warn("Could not verify token (code {}), reason is {}", result.statusCode, result.statusMessage);
  return false;
}

std::future<std::string> AuthService::signIn(const std::string &email,
                                                            const std::string &password) {
  return std::async(std::launch::async, [this, email, password] {
    return signInSync(email, password);
  });
}

std::string AuthService::signInSync(const std::string &email,
                                    const std::string &password) {
  nlohmann::json jsonBody;
  jsonBody["email"] = email;
  jsonBody["password"] = password;
  auto header = teckos::Header();
  auto result = teckos::rest::Post(this->url_ + "/login", header, jsonBody);
  if (result.statusCode == 200) {
    return result.body.get<std::string>();
  }
  throw AuthError(result.statusCode, result.statusMessage);
}

std::future<bool> AuthService::signOut(const std::string &token) {
  return std::async(std::launch::async, [this, &token] {
    return signOutSync(token);
  });
}

[[maybe_unused]] bool AuthService::signOutSync(const std::string &token) {
  auto header = teckos::Header();
  header.insert({"Authorization", "Bearer " + token});
  auto result = teckos::rest::Post(this->url_ + "/logout", header);
  if (result.statusCode == 200) {
    return true;
  }
  spdlog::error("Could not sign out (code {}), reason is {}", result.statusCode, result.statusMessage);
  return false;
}
