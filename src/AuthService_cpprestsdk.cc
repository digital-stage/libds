//
// Created by Tobias Hegemann on 24.02.21.
//
#include "DigitalStage/Auth/AuthService.h"
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <iostream>
#include <stdexcept>

using namespace web;
using namespace web::http;
using namespace web::http::client;

using namespace DigitalStage::Auth;

std::future<bool> AuthService::verifyToken(const std::string &token) {
  std::promise<bool> promise;
  auto future = promise.get_future();
  auto url = this->url_;
  pplx::create_task([url, token]() {
    http_client client(url);
    http_request request(methods::GET);
    request.
        set_request_uri(uri_builder(U("profile"))
                            .
                                to_string()
    );
    request.
            headers()
        .add(U("Content-Type"), U("application/json"));
    request.
            headers()
        .add(U("Authorization"), U("Bearer " + token));
    return client.
        request(request);
  })
      .then([&promise](
          const http_response &response
      ) {
// Check the status code.
        std::cout << response.
            status_code()
                  <<
                  std::endl;
        if (response.
            status_code()
            != 200) {
          promise.set_value(false);
        }
        // Convert the response body to JSON object.
        promise.set_value(true);
      });
  return future;
}

[[maybe_unused]] bool AuthService::verifyTokenSync(const std::string &token) {
  auto postJson = this->verifyToken(token);
  try {
    postJson.wait();
    return postJson.get();
  }
  catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}

AuthService::AuthService(const std::string &authUrl) {
  this->url_ = authUrl;
}

std::future<std::optional<std::string>> AuthService::signIn(const std::string &email,
                                                            const std::string &password) {
  std::promise<std::optional<std::string>> promise;
  auto future = promise.get_future();
  auto url = this->url_;
  pplx::create_task([url, email, password]() {
    json::value jsonObject;
    jsonObject[U("email")] = json::value::string(email);
    jsonObject[U("password")] = json::value::string(password);

    return http_client(url).request(
        methods::POST, uri_builder(U("login")).to_string(),
        jsonObject.serialize(), U("application/json"));
  })
      .then([](const http_response &response) {
        // Check the status code.
        if (response.status_code() != 200) {
          throw std::invalid_argument("Returned " +
              std::to_string(response.status_code()));
        }
        // Convert the response body to JSON object.
        return response.extract_json();
      })
          // Parse the user details.
      .then([&promise](const json::value &jsonObject) {
        promise.set_value(jsonObject.as_string());
      });
  return future;
}

std::optional<std::string> AuthService::signInSync(const std::string &email,
                                                   const std::string &password) {
  auto postJson = this->signIn(email, password);
  try {
    postJson.wait();
    return postJson.get();
  }
  catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return U("");
  }
}

std::future<bool> AuthService::signOut(const std::string &token) {
  std::promise<bool> promise;
  auto future = promise.get_future();
  const auto url = this->url_;
  pplx::create_task([url, token]() {
    http_client client(url + U("/logout"));
    http_request request(methods::POST);
    request.headers().add(U("Content-Type"), U("application/json"));
    request.headers().add(U("Authorization"), U("Bearer " + token));
    return client.request(request);
  })
      .then([&promise](const http_response &response) {
        // Check the status code.
        if (response.status_code() != 200) {
          promise.set_value(false);
        }
        // Convert the response body to JSON object.
        promise.set_value(true);
      });
  return future;
}

[[maybe_unused]] bool AuthService::signOutSync(const std::string &token) {
  auto postJson = this->signOut(token);
  try {
    postJson.wait();
    return postJson.get();
  }
  catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}