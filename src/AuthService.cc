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

namespace
{
    class HttpErrorException : public std::runtime_error
    {
    public:
        HttpErrorException(int errorCode)
            : std::runtime_error("Http Error")
            , errorCode_(errorCode)
        {}

        [[nodiscard]] int errorCode() const
        { 
            return errorCode_; 
        }

    private:
        int errorCode_;
    };
}

pplx::task<bool> AuthService::verifyToken(const string_t& token)
{
  auto url = this->url_;
  return pplx::create_task([url, token]() {
           http_client client(url);
           http_request request(methods::GET);
           request.set_request_uri(uri_builder(U("profile")).to_string());
           request.headers().add(U("Content-Type"), U("application/json"));
           request.headers().add(U("Authorization"), U("Bearer " + token));
           return client.request(request);
         })
      .then([](const http_response& response) {
        // Check the status code.
        std::cout << response.status_code() << std::endl;
        if(response.status_code() != 200) {
          return false;
        }
        // Convert the response body to JSON object.
        return true;
      });
}

[[maybe_unused]] bool AuthService::verifyTokenSync(const string_t& token)
{
  auto postJson = this->verifyToken(token);
  try {
    postJson.wait();
    return postJson.get();
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}

AuthService::AuthService(const string_t& authUrl)
{
  this->url_ = authUrl;
}

pplx::task<string_t> AuthService::signIn(const string_t& email,
                                            const string_t& password)
{
  auto url = this->url_;
  return pplx::create_task([url, email, password]() {
           json::value jsonObject;
           jsonObject[U("email")] = json::value::string(email);
           jsonObject[U("password")] = json::value::string(password);

           return http_client(url).request(
               methods::POST, uri_builder(U("login")).to_string(),
               jsonObject.serialize(), U("application/json"));
         })
      .then([](const http_response& response) {
        // Check the status code.
        if(response.status_code() != 200) {
            throw HttpErrorException(static_cast<int>(response.status_code()));
        }
        // Convert the response body to JSON object.
        return response.extract_json();
      })
      // Parse the user details.
      .then([](const json::value& jsonObject) { return jsonObject.as_string(); });
}

LoginResult AuthService::signInSync(const string_t& email,
                                    const string_t& password)
{
  auto postJson = this->signIn(email, password);
  try {
    postJson.wait();
    return LoginResult{postJson.get(), 0};
  }
  catch (const HttpErrorException & e) {
      return LoginResult{{}, e.errorCode()};
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return LoginResult{string_t{L"Unexpected Error"}, -1};
  }
}

pplx::task<bool> AuthService::signOut(const string_t& token)
{
  const string_t url = this->url_;
  return pplx::create_task([url, token]() {
           http_client client(url + U("/logout"));
           http_request request(methods::POST);
           request.headers().add(U("Content-Type"), U("application/json"));
           request.headers().add(U("Authorization"), U("Bearer " + token));
           return client.request(request);
         })
      .then([](const http_response& response) {
        // Check the status code.
        if(response.status_code() != 200) {
          return false;
        }
        // Convert the response body to JSON object.
        return true;
      });
}

[[maybe_unused]] bool AuthService::signOutSync(const string_t& token)
{
  auto postJson = this->signOut(token);
  try {
    postJson.wait();
    return postJson.get();
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  }
}