#ifndef DS_CLIENT
#define DS_CLIENT

#include "Events.h"
#include "Store.h"
#include <future>
#include <mutex>
#include <optional>
#include <sigslot/signal.hpp>
#include <teckos/client.h>
#include <optional>

namespace DigitalStage {
namespace Api {

class InvalidPayloadException : public std::runtime_error {
 public:
  InvalidPayloadException(const std::string& what = "") : std::runtime_error(what) {}
};

class Client {
 public:
  /**
   * You can use this empty struct as a type for generating a life-cycle token.
   * @see https://github.com/palacaze/sigslot#automatic-slot-lifetime-tracking
   */
  struct Token {};

  explicit Client(std::string apiUrl, bool async_events = false);
  ~Client();

  void
  connect(const std::string &apiToken,
          const nlohmann::json &initialDevicePayload = nullptr);

  void disconnect();

  Store *getStore() const;

  bool isConnected();

  void send(const std::string &event,
            const nlohmann::json &message);

  void send(const std::string &event, const nlohmann::json &message,
            teckos::Callback callback);

  sigslot::signal<bool /* expected */> disconnected;
  sigslot::signal<const DigitalStage::Api::Store *> ready;
  sigslot::signal<const Device, const DigitalStage::Api::Store *>
      localDeviceReady;
  sigslot::signal<const User, const DigitalStage::Api::Store *>
      localUserReady;

  sigslot::signal<const ID_TYPE &, const std::optional<ID_TYPE> &,
                  const DigitalStage::Api::Store *>
      stageJoined;
  sigslot::signal<const DigitalStage::Api::Store *> stageLeft;

  sigslot::signal<const Device, const DigitalStage::Api::Store *> deviceAdded;
  sigslot::signal<const ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      deviceChanged;
  sigslot::signal<const ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      localDeviceChanged;
  /**
   * This will send when the audio driver has been changed.
   * The first parameter contains the new audio driver as optional value
   */
  sigslot::signal<std::optional<std::string>,
                  const DigitalStage::Api::Store *>
      audioDriverSelected;
  /**
   * This will send when another input sound card has been selected.
   * First parameter is the ID of the sound card as optional value
   */
  sigslot::signal<const std::optional<ID_TYPE> &,
                  const DigitalStage::Api::Store *>
      inputSoundCardSelected;
  /**
   * This will send when another output sound card has been selected.
   * First parameter is the ID of the sound card as optional value
   */
  sigslot::signal<const std::optional<ID_TYPE> &,
                  const DigitalStage::Api::Store *>
      outputSoundCardSelected;
  sigslot::signal<const ID_TYPE &, const DigitalStage::Api::Store *>
      deviceRemoved;

  sigslot::signal<const Stage, const DigitalStage::Api::Store *> stageAdded;
  sigslot::signal<const ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      stageChanged;
  sigslot::signal<const ID_TYPE &, const DigitalStage::Api::Store *>
      stageRemoved;

  sigslot::signal<const Group, const DigitalStage::Api::Store *> groupAdded;
  sigslot::signal<const ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      groupChanged;
  sigslot::signal<const ID_TYPE &, const DigitalStage::Api::Store *>
      groupRemoved;

  sigslot::signal<const CustomGroup, const DigitalStage::Api::Store *> customGroupAdded;
  sigslot::signal<const ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      customGroupChanged;
  sigslot::signal<const CustomGroup, const DigitalStage::Api::Store *>
      customGroupRemoved;

  sigslot::signal<const StageMember, const DigitalStage::Api::Store *>
      stageMemberAdded;
  sigslot::signal<const ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      stageMemberChanged;
  sigslot::signal<const ID_TYPE &, const DigitalStage::Api::Store *>
      stageMemberRemoved;

  sigslot::signal<const StageDevice, const DigitalStage::Api::Store *>
      stageDeviceAdded;
  sigslot::signal<const ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      stageDeviceChanged;
  sigslot::signal<const StageDevice &, const DigitalStage::Api::Store *>
      stageDeviceRemoved;

  sigslot::signal<const SoundCard, const DigitalStage::Api::Store *>
      soundCardAdded;
  sigslot::signal<const ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      soundCardChanged;
  sigslot::signal<const ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      inputSoundCardChanged;
  sigslot::signal<const ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      outputSoundCardChanged;
  sigslot::signal<const ID_TYPE &, const DigitalStage::Api::Store *>
      soundCardRemoved;

  sigslot::signal<const VideoTrack, const DigitalStage::Api::Store *>
      videoTrackAdded;
  sigslot::signal<const ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      videoTrackChanged;
  sigslot::signal<const VideoTrack, const DigitalStage::Api::Store *>
      videoTrackRemoved;

  sigslot::signal<const AudioTrack, const DigitalStage::Api::Store *>
      audioTrackAdded;
  sigslot::signal<const ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      audioTrackChanged;
  sigslot::signal<const AudioTrack, const DigitalStage::Api::Store *>
      audioTrackRemoved;

  sigslot::signal<const User, const DigitalStage::Api::Store *> userAdded;
  sigslot::signal<const ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      userChanged;
  sigslot::signal<const ID_TYPE &, const DigitalStage::Api::Store *>
      userRemoved;

  sigslot::signal<const P2PRestart, const DigitalStage::Api::Store *>
      p2pRestart;
  sigslot::signal<const P2PAnswer, const DigitalStage::Api::Store *>
      p2pAnswer;
  sigslot::signal<const P2POffer, const DigitalStage::Api::Store *>
      p2pOffer;
  sigslot::signal<const IceCandidate, const DigitalStage::Api::Store *>
      iceCandidate;

  sigslot::signal<const std::exception &>
      error;

  /**
   * Not implemented, but could be an alternative to the store
   * @return
   */
  [[maybe_unused]] DigitalStage::Types::WholeStage getWholeStage() const;

  /**
   * Not implemented, but could be an alternative to the store
   */
  [[maybe_unused]] void setWholeStage(nlohmann::json wholeStage);

  /**
   * Decodes an invitation key.
   * Returns nothing, if invitation key is invalid or expired.
   * @return pair of stage ID and optional group ID
   */
  std::pair<std::string, std::optional<std::string>> decodeInvitationCodeSync(const std::string &code);

  /**
   * Decodes an invitation key.
   * Returns nothing, if invitation key is invalid or expired.
   * @return pair of stage ID and optional group ID
   */
  std::future<std::pair<std::string, std::optional<std::string>>> decodeInvitationCode(const std::string &code);

  /**
   * Revoke an invitation code and return a new one.
   * This invalidates the old code.
   * @param stage ID
   * @param group ID
   */
  std::string revokeInvitationCodeSync(const std::string &stageId, const std::optional<std::string> &groupId = std::nullopt);

  /**
   * Revoke an invitation code and return a new one.
   * This invalidates the old code.
   * @param stage ID
   * @param group ID
   */
  std::future<std::string> revokeInvitationCode(const std::string &stageId, const std::optional<std::string> &groupId = std::nullopt);

  std::string encodeInvitationCodeSync(const std::string &stageId, const std::optional<std::string> &groupId = std::nullopt);

  std::future<std::string> encodeInvitationCode(const std::string &stageId, const std::optional<std::string> &groupId = std::nullopt);

  void handleMessage(const std::string & event, const nlohmann::json &payload);
 private:

  const std::string apiUrl_;
  std::unique_ptr<Store> store_;
  nlohmann::json wholeStage_;
  mutable std::mutex wholeStage_mutex_;
  std::unique_ptr<teckos::client> wsclient_;
};
} // namespace Api
} // namespace DigitalStage

#endif // DS_CLIENT