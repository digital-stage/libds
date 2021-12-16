#ifndef DS_CLIENT
#define DS_CLIENT

#include <memory>                   // for unique_ptr
#include <utility>                  // for pair
#include <teckos/client.h>          // for Callback
#include <mutex>                    // for mutex
#include <future>                   // for future
#include <iosfwd>                   // for string
#include <nlohmann/json.hpp>        // for basic_json
#include <nlohmann/json_fwd.hpp>    // for json
#include <optional>                 // for optional
#include <sigslot/signal.hpp>       // for signal
#include <vector>                   // for vector
#include "DigitalStage/Api/Store.h" // for ID_TYPE, json, AudioTrack (ptr only)

namespace DigitalStage {
namespace Api {

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
  sigslot::signal<const Store *> ready;
  sigslot::signal<const Types::Device, const Store *>
      localDeviceReady;
  sigslot::signal<const Types::User, const Store *>
      localUserReady;

  sigslot::signal<const Types::ID_TYPE &, const std::optional<Types::ID_TYPE> &,
                  const Store *>
      stageJoined;
  sigslot::signal<const Store *> stageLeft;

  sigslot::signal<const Types::Device, const Store *> deviceAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      deviceChanged;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      localDeviceChanged;
  /**
   * This will send when the audio driver has been changed.
   * The first parameter contains the new audio driver as optional value
   */
  sigslot::signal<std::optional<std::string>,
                  const Store *>
      audioDriverSelected;
  /**
   * This will send when another input sound card has been selected.
   * First parameter is the ID of the sound card as optional value
   */
  sigslot::signal<const std::optional<Types::ID_TYPE> &,
                  const Store *>
      inputSoundCardSelected;
  /**
   * This will send when another output sound card has been selected.
   * First parameter is the ID of the sound card as optional value
   */
  sigslot::signal<const std::optional<Types::ID_TYPE> &,
                  const Store *>
      outputSoundCardSelected;
  sigslot::signal<const Types::ID_TYPE &, const Store *>
      deviceRemoved;

  sigslot::signal<const Types::Stage, const Store *> stageAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      stageChanged;
  sigslot::signal<const Types::ID_TYPE &, const Store *>
      stageRemoved;

  sigslot::signal<const Types::Group, const Store *> groupAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      groupChanged;
  sigslot::signal<const Types::ID_TYPE &, const Store *>
      groupRemoved;

  sigslot::signal<const Types::CustomGroup, const Store *> customGroupAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      customGroupChanged;
  sigslot::signal<const Types::CustomGroup, const Store *>
      customGroupRemoved;

  sigslot::signal<const Types::CustomGroupPosition, const Store *>
      customGroupPositionAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      customGroupPositionChanged;
  sigslot::signal<const Types::CustomGroupPosition, const Store *>
      customGroupPositionRemoved;

  sigslot::signal<const Types::CustomGroupVolume, const Store *>
      customGroupVolumeAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      customGroupVolumeChanged;
  sigslot::signal<const Types::CustomGroupVolume, const Store *>
      customGroupVolumeRemoved;

  sigslot::signal<const Types::StageMember, const Store *>
      stageMemberAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      stageMemberChanged;
  sigslot::signal<const Types::ID_TYPE &, const Store *>
      stageMemberRemoved;

  sigslot::signal<const Types::CustomStageMemberPosition,
                  const Store *>
      customStageMemberPositionAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      customStageMemberPositionChanged;
  sigslot::signal<const Types::CustomStageMemberPosition, const Store *>
      customStageMemberPositionRemoved;

  sigslot::signal<const Types::CustomStageMemberVolume,
                  const Store *>
      customStageMemberVolumeAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      customStageMemberVolumeChanged;
  sigslot::signal<const Types::CustomStageMemberVolume, const Store *>
      customStageMemberVolumeRemoved;

  sigslot::signal<const Types::StageDevice, const Store *>
      stageDeviceAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      stageDeviceChanged;
  sigslot::signal<const Types::StageDevice &, const Store *>
      stageDeviceRemoved;

  sigslot::signal<const Types::CustomStageDevicePosition,
                  const Store *>
      customStageDevicePositionAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      customStageDevicePositionChanged;
  sigslot::signal<const Types::CustomStageDevicePosition, const Store *>
      customStageDevicePositionRemoved;

  sigslot::signal<const Types::CustomStageDeviceVolume,
                  const Store *>
      customStageDeviceVolumeAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      customStageDeviceVolumeChanged;
  sigslot::signal<const Types::CustomStageDeviceVolume, const Store *>
      customStageDeviceVolumeRemoved;

  sigslot::signal<const Types::SoundCard, const Store *>
      soundCardAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      soundCardChanged;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      inputSoundCardChanged;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      outputSoundCardChanged;
  sigslot::signal<const Types::ID_TYPE &, const Store *>
      soundCardRemoved;

  sigslot::signal<const Types::VideoTrack, const Store *>
      videoTrackAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      videoTrackChanged;
  sigslot::signal<const Types::VideoTrack, const Store *>
      videoTrackRemoved;

  sigslot::signal<const Types::AudioTrack, const Store *>
      audioTrackAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      audioTrackChanged;
  sigslot::signal<const Types::AudioTrack, const Store *>
      audioTrackRemoved;

  sigslot::signal<const Types::CustomAudioTrackPosition,
                  const Store *>
      customAudioTrackPositionAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      customAudioTrackPositionChanged;
  sigslot::signal<const Types::CustomAudioTrackPosition, const Store *>
      customAudioTrackPositionRemoved;

  sigslot::signal<const Types::CustomAudioTrackVolume,
                  const Store *>
      customAudioTrackVolumeAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      customAudioTrackVolumeChanged;
  sigslot::signal<const Types::CustomAudioTrackVolume, const Store *>
      customAudioTrackVolumeRemoved;

  sigslot::signal<const Types::User, const Store *> userAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const Store *>
      userChanged;
  sigslot::signal<const Types::ID_TYPE &, const Store *>
      userRemoved;

  sigslot::signal<const Types::P2PRestart, const Store *>
      p2pRestart;
  sigslot::signal<const Types::P2PAnswer, const Store *>
      p2pAnswer;
  sigslot::signal<const Types::P2POffer, const Store *>
      p2pOffer;
  sigslot::signal<const Types::IceCandidate, const Store *>
      iceCandidate;

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
   * @return pair of stage ID and group ID
   */
  std::pair<std::string, std::string> decodeInvitationCodeSync(const std::string &code);

  /**
   * Decodes an invitation key.
   * Returns nothing, if invitation key is invalid or expired.
   * @return pair of stage ID and group ID
   */
  std::future<std::pair<std::string, std::string>> decodeInvitationCode(const std::string &code);

  /**
   * Revoke an invitation code and return a new one.
   * This invalidates the old code.
   * @param stage ID
   * @param group ID
   */
  std::string revokeInvitationCodeSync(const std::string &stageId, const std::string &groupId);

  /**
   * Revoke an invitation code and return a new one.
   * This invalidates the old code.
   * @param stage ID
   * @param group ID
   */
  std::future<std::string> revokeInvitationCode(const std::string &stageId, const std::string &groupId);

  std::string encodeInvitationCodeSync(const std::string &stageId, const std::string &groupId);

  std::future<std::string> encodeInvitationCode(const std::string &stageId, const std::string &groupId);

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