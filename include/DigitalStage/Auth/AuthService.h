//
// Created by Tobias Hegemann on 24.02.21.
//

#ifndef DS_AUTH_SERVICE_H
#define DS_AUTH_SERVICE_H

#include <pplx/pplxtasks.h>
#include <cpprest/uri.h>

namespace DigitalStage::Auth {

    typedef utility::string_t string_t;

    struct LoginResult
    {
        string_t token;
        int httpError; // error code or 0 for no error
    };

    class AuthService {
    public:
      /**
       * Create a new auth service class by using the given url to the auth
       * service
       * @param authUrl
       */
      explicit AuthService(const string_t& authUrl);

      /**
       * Verify the given token
       * @param token
       * @return task object, use wait() and get() on it to receive the bool
       * value
       */
      pplx::task<bool> verifyToken(const string_t& token);

      /**
       * Verify the given token
       * @param token
       * @return true if token is valid, otherwise false
       */
      [[maybe_unused]] bool verifyTokenSync(const string_t& token);

      /**
       * Sign into the auth service with given email and password
       * @param email
       * @param password
       * @return task object, use wait() and get() on it to receive the value
       */
      pplx::task<string_t> signIn(const string_t& email,
                                     const string_t& password);

      /**
       * Sign into the auth service with given email and password
       * @param email
       * @param password
       * @return valid token when successful, otherwise empty string
       */
      LoginResult signInSync(const string_t& email,
                             const string_t& password);

      /**
       * Sign out of the auth service and invalidate the given token
       * @param token
       * @return task object, use wait() and get() on it to receive the bool
       * value
       */
      pplx::task<bool> signOut(const string_t& token);

      /**
       * Sign out of the auth service and invalidate the given token
       * @param token
       * @return true if successful, otherwise false
       */
      [[maybe_unused]] bool signOutSync(const string_t& token);

    private:
      string_t url_;
    };
  } // namespace DigitalStage

#endif // DS_AUTH_SERVICE_H
