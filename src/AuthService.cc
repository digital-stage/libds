//
// Created by Tobias Hegemann on 24.02.21.
//
#include "AuthService.h"
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <iostream>
#include <stdexcept>

using namespace web;
using namespace web::http;
using namespace web::http::client;

pplx::task<bool>
DigitalStage::AuthService::verifyToken(const std::string& token)
{
  const std::string url = this->url;
  return pplx::create_task([url, token]() {
           http_client client(url);
           http_request request(methods::POST);
           request.set_request_uri(uri_builder(U("profile")).to_string());
           request.headers().add(U("Content-Type"), U("application/json"));
           request.headers().add(U("Authorization"), U("Bearer " + token));
           return client.request(request);
           /*json::value jsonObject;
           jsonObject[U("Authorization")] =
               json::value::string(U("Bearer " + token));
            return http_client(U(url)).request(
               methods::POST, uri_builder(U("profile")).to_string(),
               jsonObject.serialize(), U("application/json"));
           */
         })
      .then([](http_response response) {
        // Check the status code.
        std::cout << response.status_code() << std::endl;
        if(response.status_code() != 200) {
          return false;
        }
        // Convert the response body to JSON object.
        return true;
      });
}

bool DigitalStage::AuthService::verifyTokenSync(const std::string& token)
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

DigitalStage::AuthService::AuthService(const std::string& authUrl)
{
  this->url = authUrl;
}

pplx::task<std::string>
DigitalStage::AuthService::signIn(const std::string& email,
                                  const std::string& password)
{
  const std::string url = this->url;
  return pplx::create_task([url, email, password]() {
           json::value jsonObject;
           jsonObject[U("email")] = json::value::string(U(email));
           jsonObject[U("password")] = json::value::string(U(password));

           return http_client(U(url)).request(
               methods::POST, uri_builder(U("login")).to_string(),
               jsonObject.serialize(), U("application/json"));
         })
      .then([](http_response response) {
        // Check the status code.
        if(response.status_code() != 200) {
          throw std::invalid_argument("Returned " +
                                      std::to_string(response.status_code()));
        }
        // Convert the response body to JSON object.
        return response.extract_json();
      })
      // Parse the user details.
      .then([](json::value jsonObject) { return jsonObject.as_string(); });
}

std::string DigitalStage::AuthService::signInSync(const std::string& email,
                                                  const std::string& password)
{
  auto postJson = this->signIn(email, password);
  try {
    postJson.wait();
    return postJson.get();
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return "";
  }
}

pplx::task<bool> DigitalStage::AuthService::signOut(const std::string& token)
{
  const std::string url = this->url;
  return pplx::create_task([url, token]() {
           http_client client(U(url + "/logout"));
           http_request request(methods::POST);
           request.headers().add(U("Content-Type"), U("application/json"));
           request.headers().add(U("Authorization"), U("Bearer " + token));
           return client.request(request);
         })
      .then([](http_response response) {
        // Check the status code.
        if(response.status_code() != 200) {
          return false;
        }
        // Convert the response body to JSON object.
        return true;
      });
}

bool DigitalStage::AuthService::signOutSync(const std::string& token)
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