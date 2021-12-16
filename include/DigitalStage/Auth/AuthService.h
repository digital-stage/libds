//
// Created by Tobias Hegemann on 24.02.21.
//

#ifndef DS_AUTH_SERVICE_H
#define DS_AUTH_SERVICE_H

#include <future>  // for future
#include <iosfwd>  // for string

namespace DigitalStage {
namespace Auth {

class AuthService {
 public:
  /**
   * Create a new auth service class by using the given url to the auth
   * service
   * @param authUrl
   */
  explicit AuthService(const std::string &authUrl);

  /**
   * Verify the given token
   * @param token
   * @return task object, use wait() and get() on it to receive the bool
   * value
   */
  std::future<bool> verifyToken(const std::string &token);

  /**
   * Verify the given token
   * @param token
   * @return true if token is valid, otherwise false
   */
  [[maybe_unused]] bool verifyTokenSync(const std::string &token);

  /**
   * Sign into the auth service with given email and password
   * @param email
   * @param password
   * @return task object, use wait() and get() on it to receive the value
   * @throws AuthError
   */
  std::future<std::string> signIn(const std::string &email,
                                                 const std::string &password);

  /**
   * Sign into the auth service with given email and password
   * @param email
   * @param password
   * @return valid token when successful, will throw exception otherwise
   * @throws AuthError
   */
  std::string signInSync(const std::string &email,
                         const std::string &password);

  /**
   * Sign out of the auth service and invalidate the given token
   * @param token
   * @return task object, use wait() and get() on it to receive the bool
   * value
   */
  std::future<bool> signOut(const std::string &token);

  /**
   * Sign out of the auth service and invalidate the given token
   * @param token
   * @return true if successful, otherwise false
   */
  [[maybe_unused]] bool signOutSync(const std::string &token);

 private:
  std::string url_;
};
} // namespace Auth
} // namespace DigitalStage

#endif // DS_AUTH_SERVICE_H
