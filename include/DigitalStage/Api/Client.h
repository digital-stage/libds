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
    class Client {
    public:
      explicit Client(std::string apiUrl);

      void
      connect(const std::string& apiToken,
              const nlohmann::json& initialDevicePayload = nullptr);

      void disconnect();

      Store* getStore() const;

      bool isConnected();

      void send(const std::string& event,
                const nlohmann::json& message);

      void
      send(const std::string& event, const nlohmann::json& message,
           const std::function<void(const std::vector<nlohmann::json>&)>&
               callback);

      Pal::sigslot::signal<bool /* expected */> disconnected;
      Pal::sigslot::signal<const DigitalStage::Api::Store*> ready;
      Pal::sigslot::signal<const Device, const DigitalStage::Api::Store*>
          localDeviceReady;
      Pal::sigslot::signal<const User, const DigitalStage::Api::Store*>
          localUserReady;

      Pal::sigslot::signal<const ID_TYPE&, const std::optional<ID_TYPE> &,
                           const DigitalStage::Api::Store*>
          stageJoined;
      Pal::sigslot::signal<const DigitalStage::Api::Store*> stageLeft;

      Pal::sigslot::signal<const Device, const DigitalStage::Api::Store*> deviceAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          deviceChanged;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          localDeviceChanged;
      /**
       * This will send when the audio driver has been changed.
       * The first parameter contains the new audio driver as optional value
       */
      Pal::sigslot::signal<std::optional<std::string>,
                           const DigitalStage::Api::Store*>
          audioDriverSelected;
      /**
       * This will send when another input sound card has been selected.
       * First parameter is the ID of the sound card as optional value
       */
      Pal::sigslot::signal<const std::optional<std::string>&,
                           const DigitalStage::Api::Store*>
          inputSoundCardSelected;
      /**
       * This will send when another output sound card has been selected.
       * First parameter is the ID of the sound card as optional value
       */
      Pal::sigslot::signal<const std::optional<std::string>&,
                           const DigitalStage::Api::Store*>
          outputSoundCardSelected;
      Pal::sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          deviceRemoved;

      Pal::sigslot::signal<const Stage, const DigitalStage::Api::Store*> stageAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          stageChanged;
      Pal::sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          stageRemoved;

      Pal::sigslot::signal<const Group, const DigitalStage::Api::Store*> groupAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          groupChanged;
      Pal::sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          groupRemoved;

      Pal::sigslot::signal<const CustomGroupPosition, const DigitalStage::Api::Store*>
          customGroupPositionAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          customGroupPositionChanged;
      Pal::sigslot::signal<const CustomGroupPosition, const DigitalStage::Api::Store*>
          customGroupPositionRemoved;

      Pal::sigslot::signal<const CustomGroupVolume, const DigitalStage::Api::Store*>
          customGroupVolumeAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          customGroupVolumeChanged;
      Pal::sigslot::signal<const CustomGroupVolume, const DigitalStage::Api::Store*>
          customGroupVolumeRemoved;

      Pal::sigslot::signal<const StageMember, const DigitalStage::Api::Store*>
          stageMemberAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          stageMemberChanged;
      Pal::sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          stageMemberRemoved;

      Pal::sigslot::signal<const CustomStageMemberPosition,
                           const DigitalStage::Api::Store*>
          customStageMemberPositionAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          customStageMemberPositionChanged;
      Pal::sigslot::signal<const CustomStageMemberPosition, const DigitalStage::Api::Store*>
          customStageMemberPositionRemoved;

      Pal::sigslot::signal<const CustomStageMemberVolume,
                           const DigitalStage::Api::Store*>
          customStageMemberVolumeAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          customStageMemberVolumeChanged;
      Pal::sigslot::signal<const CustomStageMemberVolume, const DigitalStage::Api::Store*>
          customStageMemberVolumeRemoved;

      Pal::sigslot::signal<const StageDevice, const DigitalStage::Api::Store*>
          stageDeviceAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          stageDeviceChanged;
      Pal::sigslot::signal<const StageDevice&, const DigitalStage::Api::Store*>
          stageDeviceRemoved;

      Pal::sigslot::signal<const CustomStageDevicePosition,
                           const DigitalStage::Api::Store*>
          customStageDevicePositionAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          customStageDevicePositionChanged;
      Pal::sigslot::signal<const CustomStageDevicePosition, const DigitalStage::Api::Store*>
          customStageDevicePositionRemoved;

      Pal::sigslot::signal<const CustomStageDeviceVolume,
                           const DigitalStage::Api::Store*>
          customStageDeviceVolumeAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          customStageDeviceVolumeChanged;
      Pal::sigslot::signal<const CustomStageDeviceVolume, const DigitalStage::Api::Store*>
          customStageDeviceVolumeRemoved;

      Pal::sigslot::signal<const SoundCard, const DigitalStage::Api::Store*>
          soundCardAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          soundCardChanged;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          inputSoundCardChanged;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          outputSoundCardChanged;
      Pal::sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          soundCardRemoved;

      Pal::sigslot::signal<const VideoTrack, const DigitalStage::Api::Store*>
          videoTrackAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          videoTrackChanged;
      Pal::sigslot::signal<const VideoTrack, const DigitalStage::Api::Store*>
          videoTrackRemoved;

      Pal::sigslot::signal<const AudioTrack, const DigitalStage::Api::Store*>
          audioTrackAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          audioTrackChanged;
      Pal::sigslot::signal<const AudioTrack, const DigitalStage::Api::Store*>
          audioTrackRemoved;

      Pal::sigslot::signal<const CustomAudioTrackPosition,
                           const DigitalStage::Api::Store*>
          customAudioTrackPositionAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          customAudioTrackPositionChanged;
      Pal::sigslot::signal<const CustomAudioTrackPosition, const DigitalStage::Api::Store*>
          customAudioTrackPositionRemoved;

      Pal::sigslot::signal<const CustomAudioTrackVolume,
                           const DigitalStage::Api::Store*>
          customAudioTrackVolumeAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          customAudioTrackVolumeChanged;
      Pal::sigslot::signal<const CustomAudioTrackVolume, const DigitalStage::Api::Store*>
          customAudioTrackVolumeRemoved;

      Pal::sigslot::signal<const User, const DigitalStage::Api::Store*> userAdded;
      Pal::sigslot::signal<const std::string&, nlohmann::json,
                           const DigitalStage::Api::Store*>
          userChanged;
      Pal::sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          userRemoved;

      Pal::sigslot::signal<const P2PRestart, const DigitalStage::Api::Store*>
          p2pRestart;
      Pal::sigslot::signal<const P2PAnswer, const DigitalStage::Api::Store*>
          p2pAnswer;
      Pal::sigslot::signal<const P2POffer, const DigitalStage::Api::Store*>
          p2pOffer;
      Pal::sigslot::signal<const IceCandidate, const DigitalStage::Api::Store*>
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
      std::future<std::pair<std::string, std::string>> decodeInvitationCode(const std::string& code);

      /**
       * Revoke an invitation code and return a new one.
       * This invalidates the old code.
       * @param stage ID
       * @param group ID
       */
      std::future<std::string> revokeInvitationCode(const std::string& stageId, const std::string& groupId);

      std::future<std::string> encodeInvitationCode(const std::string& stageId, const std::string& groupId);

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