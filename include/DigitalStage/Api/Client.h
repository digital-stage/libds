#ifndef DS_CLIENT
#define DS_CLIENT

#include "Events.h"
#include "Store.h"
#include <mutex>
#include <pplx/pplxtasks.h>
#include <sigslot/signal.hpp>
#include <teckos/client.h>

using namespace web::websockets::client;

namespace DigitalStage::Api {
    class Client {
    public:
      explicit Client(std::string apiUrl);

      pplx::task<void> connect(const std::string& apiToken,
                               const nlohmann::json& initialDevicePayload = nullptr);

      void disconnect();

      Store* getStore() const;

      bool isConnected();

      pplx::task<void> send(const std::string& event,
                            const nlohmann::json& message);

      pplx::task<void>
      send(const std::string& event, const nlohmann::json& message,
           const std::function<void(const std::vector<nlohmann::json>&)>&
               callback);

      sigslot::signal<const DigitalStage::Api::Store*> ready;
      sigslot::signal<const Device, const DigitalStage::Api::Store*>
          localDeviceReady;
      sigslot::signal<const user_t, const DigitalStage::Api::Store*>
          localUserReady;

      sigslot::signal<const ID_TYPE&, const ID_TYPE&,
                      const DigitalStage::Api::Store*>
          stageJoined;
      sigslot::signal<const DigitalStage::Api::Store*> stageLeft;

      sigslot::signal<const Device, const DigitalStage::Api::Store*>
          deviceAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          deviceChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          deviceRemoved;

      sigslot::signal<const Stage, const DigitalStage::Api::Store*> stageAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          stageChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          stageRemoved;

      sigslot::signal<const Group, const DigitalStage::Api::Store*> groupAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          groupChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          groupRemoved;

      sigslot::signal<const CustomGroupPosition,
                      const DigitalStage::Api::Store*>
          customGroupPositionAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          customGroupPositionChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          customGroupPositionRemoved;

      sigslot::signal<const CustomGroupVolume, const DigitalStage::Api::Store*>
          customGroupVolumeAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          customGroupVolumeChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          customGroupVolumeRemoved;

      sigslot::signal<const StageMember, const DigitalStage::Api::Store*>
          stageMemberAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          stageMemberChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          stageMemberRemoved;

      sigslot::signal<const custom_stage_member_position_t,
                      const DigitalStage::Api::Store*>
          customStageMemberPositionAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          customStageMemberPositionChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          customStageMemberPositionRemoved;

      sigslot::signal<const custom_stage_member_volume_t,
                      const DigitalStage::Api::Store*>
          customStageMemberVolumeAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          customStageMemberVolumeChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          customStageMemberVolumeRemoved;

      sigslot::signal<const soundcard_t, const DigitalStage::Api::Store*>
          soundCardAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          soundCardChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          soundCardRemoved;

      sigslot::signal<const local_video_track_t,
                      const DigitalStage::Api::Store*>
          localVideoTrackAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          localVideoTrackChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          localVideoTrackRemoved;

      sigslot::signal<const local_audio_track_t,
                      const DigitalStage::Api::Store*>
          localAudioTrackAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          localAudioTrackChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          localAudioTrackRemoved;

      sigslot::signal<const remote_video_track_t,
                      const DigitalStage::Api::Store*>
          remoteVideoTrackAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          remoteVideoTrackChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          remoteVideoTrackRemoved;

      sigslot::signal<const remote_audio_track_t,
                      const DigitalStage::Api::Store*>
          remoteAudioTrackAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          remoteAudioTrackChanged;
      sigslot::signal<const remote_audio_track_t, const DigitalStage::Api::Store*>
          remoteAudioTrackRemoved;

      sigslot::signal<const custom_remote_audio_track_position_t,
                      const DigitalStage::Api::Store*>
          customRemoteAudioTrackPositionAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          customRemoteAudioTrackPositionChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          customRemoteAudioTrackPositionRemoved;

      sigslot::signal<const custom_remote_audio_track_volume_t,
                      const DigitalStage::Api::Store*>
          customRemoteAudioTrackVolumeAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          customRemoteAudioTrackVolumeChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          customRemoteAudioTrackVolumeRemoved;

      sigslot::signal<const user_t, const DigitalStage::Api::Store*> userAdded;
      sigslot::signal<const std::string&, nlohmann::json,
                      const DigitalStage::Api::Store*>
          userChanged;
      sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
          userRemoved;

    private:
      const std::string apiUrl_;
      std::unique_ptr<Store> store_;
      std::unique_ptr<teckos::client> wsclient_;
    };
  } // namespace DigitalStage

#endif // DS_CLIENT