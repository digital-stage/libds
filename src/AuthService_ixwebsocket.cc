//
// Created by Tobias Hegemann on 24.02.21.
//
#include "DigitalStage/Auth/AuthService.h"
#include <ixwebsocket/IXHttpClient.h>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <iostream>
#include <optional>

using namespace DigitalStage::Auth;

std::future<bool> AuthService::verifyToken(const std::string &token) {
  return std::async(std::launch::async, [this, &token] {
    return verifyTokenSync(token);
  });
}

bool AuthService::verifyTokenSync(const std::string &token) {
  ix::HttpClient httpClient(false);
  ix::HttpRequestArgsPtr args_ptr;
  args_ptr.reset(new ix::HttpRequestArgs());
  args_ptr->extraHeaders.insert({{"Content-Type", "application/json"},
                                 {"Authorization", "Bearer " + token}});
  auto response = httpClient.get(this->url_ + "/profile", args_ptr);
  if (response->statusCode == 0)
    throw std::runtime_error(response->errorMsg);
  if (response->statusCode != 200)
    return false;
  return true;
}

AuthService::AuthService(const std::string &authUrl) {
  this->url_ = authUrl;
}

std::future<std::optional<std::string>> AuthService::signIn(const std::string &email,
                                                            const std::string &password) {
  return std::async(std::launch::async, [this, &email, &password] {
    return signInSync(email, password);
  });
}

std::optional<std::string> AuthService::signInSync(const std::string &email,
                                                   const std::string &password) {
  nlohmann::json jsonObject;
  jsonObject["email"] = email;
  jsonObject["password"] = password;
  ix::HttpClient httpClient(false);
  ix::HttpRequestArgsPtr args_ptr;
  args_ptr.reset(new ix::HttpRequestArgs());
  args_ptr->extraHeaders.insert({"Content-Type", "application/json"});
  auto response = httpClient.post(this->url_ + "/login", jsonObject.dump(), args_ptr);
  if (response->statusCode == 0)
    throw std::runtime_error(response->errorMsg);
  if (response->statusCode != 200 || response->body.size() < 3)
    return std::nullopt;
  // We have to trim "'s, since the result is usually "<code>"
  auto token = response->body.substr(1, response->body.size() - 2);
  std::cout << "Got token: " << token << std::endl;
  return std::optional<std::string>(token);
}

std::future<bool> AuthService::signOut(const std::string &token) {
  return std::async(std::launch::async, [this, &token] {
    return signOutSync(token);
  });
}

bool AuthService::signOutSync(const std::string &token) {
  ix::HttpClient httpClient(false);
  ix::HttpRequestArgsPtr args_ptr;
  args_ptr.reset(new ix::HttpRequestArgs());
  args_ptr->extraHeaders.insert({{"Content-Type", "application/json"},
                                 {"Authorization", "Bearer " + token}});
  auto response = httpClient.get(this->url_ + "/logout", args_ptr);
  if (response->statusCode != 200)
    return false;
  return true;
}