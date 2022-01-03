#ifndef DS_CLIENT
#define DS_CLIENT

#include <memory>                 // for unique_ptr
#include <utility>                // for pair
#include <teckos/client.h>        // for Callback
#include <__mutex_base>           // for mutex
#include <future>                 // for future
#include <iosfwd>                 // for string
#include <nlohmann/json.hpp>      // for basic_json
#include <nlohmann/json_fwd.hpp>  // for json
#include <optional>               // for optional
#include <sigslot/signal.hpp>     // for signal
#include <vector>                 // for vector
#include "DigitalStage/Types.h"   // for ID_TYPE, json, AudioTrack (ptr only)
namespace DigitalStage::Api { class Store; }

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
  sigslot::signal<const DigitalStage::Api::Store *> ready;
  sigslot::signal<const Types::Device, const DigitalStage::Api::Store *>
      localDeviceReady;
  sigslot::signal<const Types::User, const DigitalStage::Api::Store *>
      localUserReady;

  sigslot::signal<const Types::ID_TYPE &, const std::optional<Types::ID_TYPE> &,
                  const DigitalStage::Api::Store *>
      stageJoined;
  sigslot::signal<const DigitalStage::Api::Store *> stageLeft;

  sigslot::signal<const Types::Device, const DigitalStage::Api::Store *> deviceAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      deviceChanged;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
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
  sigslot::signal<const std::optional<Types::ID_TYPE> &,
                  const DigitalStage::Api::Store *>
      inputSoundCardSelected;
  /**
   * This will send when another output sound card has been selected.
   * First parameter is the ID of the sound card as optional value
   */
  sigslot::signal<const std::optional<Types::ID_TYPE> &,
                  const DigitalStage::Api::Store *>
      outputSoundCardSelected;
  sigslot::signal<const Types::ID_TYPE &, const DigitalStage::Api::Store *>
      deviceRemoved;

  sigslot::signal<const Types::Stage, const DigitalStage::Api::Store *> stageAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      stageChanged;
  sigslot::signal<const Types::ID_TYPE &, const DigitalStage::Api::Store *>
      stageRemoved;

  sigslot::signal<const Types::Group, const DigitalStage::Api::Store *> groupAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      groupChanged;
  sigslot::signal<const Types::ID_TYPE &, const DigitalStage::Api::Store *>
      groupRemoved;

  sigslot::signal<const Types::CustomGroup, const DigitalStage::Api::Store *> customGroupAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      customGroupChanged;
  sigslot::signal<const Types::CustomGroup, const DigitalStage::Api::Store *>
      customGroupRemoved;

  sigslot::signal<const Types::CustomGroupPosition, const DigitalStage::Api::Store *>
      customGroupPositionAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      customGroupPositionChanged;
  sigslot::signal<const Types::CustomGroupPosition, const DigitalStage::Api::Store *>
      customGroupPositionRemoved;

  sigslot::signal<const Types::CustomGroupVolume, const DigitalStage::Api::Store *>
      customGroupVolumeAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      customGroupVolumeChanged;
  sigslot::signal<const Types::CustomGroupVolume, const DigitalStage::Api::Store *>
      customGroupVolumeRemoved;

  sigslot::signal<const Types::StageMember, const DigitalStage::Api::Store *>
      stageMemberAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      stageMemberChanged;
  sigslot::signal<const Types::ID_TYPE &, const DigitalStage::Api::Store *>
      stageMemberRemoved;

  sigslot::signal<const Types::CustomStageMemberPosition,
                  const DigitalStage::Api::Store *>
      customStageMemberPositionAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      customStageMemberPositionChanged;
  sigslot::signal<const Types::CustomStageMemberPosition, const DigitalStage::Api::Store *>
      customStageMemberPositionRemoved;

  sigslot::signal<const Types::CustomStageMemberVolume,
                  const DigitalStage::Api::Store *>
      customStageMemberVolumeAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      customStageMemberVolumeChanged;
  sigslot::signal<const Types::CustomStageMemberVolume, const DigitalStage::Api::Store *>
      customStageMemberVolumeRemoved;

  sigslot::signal<const Types::StageDevice, const DigitalStage::Api::Store *>
      stageDeviceAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      stageDeviceChanged;
  sigslot::signal<const Types::StageDevice &, const DigitalStage::Api::Store *>
      stageDeviceRemoved;

  sigslot::signal<const Types::CustomStageDevicePosition,
                  const DigitalStage::Api::Store *>
      customStageDevicePositionAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      customStageDevicePositionChanged;
  sigslot::signal<const Types::CustomStageDevicePosition, const DigitalStage::Api::Store *>
      customStageDevicePositionRemoved;

  sigslot::signal<const Types::CustomStageDeviceVolume,
                  const DigitalStage::Api::Store *>
      customStageDeviceVolumeAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      customStageDeviceVolumeChanged;
  sigslot::signal<const Types::CustomStageDeviceVolume, const DigitalStage::Api::Store *>
      customStageDeviceVolumeRemoved;

  sigslot::signal<const Types::SoundCard, const DigitalStage::Api::Store *>
      soundCardAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      soundCardChanged;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      inputSoundCardChanged;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      outputSoundCardChanged;
  sigslot::signal<const Types::ID_TYPE &, const DigitalStage::Api::Store *>
      soundCardRemoved;

  sigslot::signal<const Types::VideoTrack, const DigitalStage::Api::Store *>
      videoTrackAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      videoTrackChanged;
  sigslot::signal<const Types::VideoTrack, const DigitalStage::Api::Store *>
      videoTrackRemoved;

  sigslot::signal<const Types::AudioTrack, const DigitalStage::Api::Store *>
      audioTrackAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      audioTrackChanged;
  sigslot::signal<const Types::AudioTrack, const DigitalStage::Api::Store *>
      audioTrackRemoved;

  sigslot::signal<const Types::CustomAudioTrackPosition,
                  const DigitalStage::Api::Store *>
      customAudioTrackPositionAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      customAudioTrackPositionChanged;
  sigslot::signal<const Types::CustomAudioTrackPosition, const DigitalStage::Api::Store *>
      customAudioTrackPositionRemoved;

  sigslot::signal<const Types::CustomAudioTrackVolume,
                  const DigitalStage::Api::Store *>
      customAudioTrackVolumeAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      customAudioTrackVolumeChanged;
  sigslot::signal<const Types::CustomAudioTrackVolume, const DigitalStage::Api::Store *>
      customAudioTrackVolumeRemoved;

  sigslot::signal<const Types::User, const DigitalStage::Api::Store *> userAdded;
  sigslot::signal<const Types::ID_TYPE &, nlohmann::json,
                  const DigitalStage::Api::Store *>
      userChanged;
  sigslot::signal<const Types::ID_TYPE &, const DigitalStage::Api::Store *>
      userRemoved;

  sigslot::signal<const Types::P2PRestart, const DigitalStage::Api::Store *>
      p2pRestart;
  sigslot::signal<const Types::P2PAnswer, const DigitalStage::Api::Store *>
      p2pAnswer;
  sigslot::signal<const Types::P2POffer, const DigitalStage::Api::Store *>
      p2pOffer;
  sigslot::signal<const Types::IceCandidate, const DigitalStage::Api::Store *>
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