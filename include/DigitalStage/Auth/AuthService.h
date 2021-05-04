//
// Created by Tobias Hegemann on 24.02.21.
//

#ifndef DS_AUTH_SERVICE_H
#define DS_AUTH_SERVICE_H

#include <pplx/pplxtasks.h>
#include <string>

namespace DigitalStage {
  namespace Auth {
    class AuthService {
    public:
      /**
       * Create a new auth service class by using the given url to the auth
       * service
       * @param authUrl
       */
      explicit AuthService(const std::string& authUrl);

      /**
       * Verify the given token
       * @param token
       * @return task object, use wait() and get() on it to receive the bool
       * value
       */
      pplx::task<bool> verifyToken(const std::string& token);

      /**
       * Verify the given token
       * @param token
       * @return true if token is valid, otherwise false
       */
      bool verifyTokenSync(const std::string& token);

      /**
       * Sign into the auth service with given email and password
       * @param email
       * @param password
       * @return task object, use wait() and get() on it to receive the value
       */
      pplx::task<std::string> signIn(const std::string& email,
                                     const std::string& password);

      /**
       * Sign into the auth service with given email and password
       * @param email
       * @param password
       * @return valid token when successful, otherwise empty string
       */
      std::string signInSync(const std::string& email,
                             const std::string& password);

      /**
       * Sign out of the auth service and invalidate the given token
       * @param token
       * @return task object, use wait() and get() on it to receive the bool
       * value
       */
      pplx::task<bool> signOut(const std::string& token);

      /**
       * Sign out of the auth service and invalidate the given token
       * @param token
       * @return true if successful, otherwise false
       */
      bool signOutSync(const std::string& token);

    private:
      std::string url;
    };
  } // namespace Auth
} // namespace DigitalStage

#endif // DS_AUTH_SERVICE_H
