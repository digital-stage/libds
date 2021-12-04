#pragma once

#include <stdexcept>
#include <string>

namespace DigitalStage {
namespace Auth {
class AuthError : public std::runtime_error {
 public:
  AuthError(int code, const std::string &what = "") : std::runtime_error(what), code_(code) {}

  inline int getCode() const {
    return code_;
  }

 private:
  const int code_;
};
} // namespace Auth
} // namespace DigitalStage