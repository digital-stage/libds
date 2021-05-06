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

    pplx::task<void>
    connect(const std::string& apiToken,
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
    sigslot::signal<const User, const DigitalStage::Api::Store*>
        localUserReady;

    sigslot::signal<const ID_TYPE&, const ID_TYPE&,
                    const DigitalStage::Api::Store*>
        stageJoined;
    sigslot::signal<const DigitalStage::Api::Store*> stageLeft;

    sigslot::signal<const Device, const DigitalStage::Api::Store*> deviceAdded;
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

    sigslot::signal<const CustomGroupPosition, const DigitalStage::Api::Store*>
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

    sigslot::signal<const CustomStageMemberPosition,
                    const DigitalStage::Api::Store*>
        customStageMemberPositionAdded;
    sigslot::signal<const std::string&, nlohmann::json,
                    const DigitalStage::Api::Store*>
        customStageMemberPositionChanged;
    sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
        customStageMemberPositionRemoved;

    sigslot::signal<const CustomStageMemberVolume,
                    const DigitalStage::Api::Store*>
        customStageMemberVolumeAdded;
    sigslot::signal<const std::string&, nlohmann::json,
                    const DigitalStage::Api::Store*>
        customStageMemberVolumeChanged;
    sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
        customStageMemberVolumeRemoved;

    sigslot::signal<const StageDevice, const DigitalStage::Api::Store*>
        stageDeviceAdded;
    sigslot::signal<const std::string&, nlohmann::json,
                    const DigitalStage::Api::Store*>
        stageDeviceChanged;
    sigslot::signal<const StageDevice&, const DigitalStage::Api::Store*>
        stageDeviceRemoved;

    sigslot::signal<const CustomStageDevicePosition,
                    const DigitalStage::Api::Store*>
        customStageDevicePositionAdded;
    sigslot::signal<const std::string&, nlohmann::json,
                    const DigitalStage::Api::Store*>
        customStageDevicePositionChanged;
    sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
        customStageDevicePositionRemoved;

    sigslot::signal<const CustomStageDeviceVolume,
                    const DigitalStage::Api::Store*>
        customStageDeviceVolumeAdded;
    sigslot::signal<const std::string&, nlohmann::json,
                    const DigitalStage::Api::Store*>
        customStageDeviceVolumeChanged;
    sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
        customStageDeviceVolumeRemoved;

    sigslot::signal<const SoundCard, const DigitalStage::Api::Store*>
        soundCardAdded;
    sigslot::signal<const std::string&, nlohmann::json,
                    const DigitalStage::Api::Store*>
        soundCardChanged;
    sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
        soundCardRemoved;
    
    sigslot::signal<const VideoTrack, const DigitalStage::Api::Store*>
        videoTrackAdded;
    sigslot::signal<const std::string&, nlohmann::json,
                    const DigitalStage::Api::Store*>
        videoTrackChanged;
    sigslot::signal<const VideoTrack, const DigitalStage::Api::Store*>
        videoTrackRemoved;

    sigslot::signal<const AudioTrack, const DigitalStage::Api::Store*>
        audioTrackAdded;
    sigslot::signal<const std::string&, nlohmann::json,
                    const DigitalStage::Api::Store*>
        audioTrackChanged;
    sigslot::signal<const AudioTrack, const DigitalStage::Api::Store*>
        audioTrackRemoved;

    sigslot::signal<const CustomAudioTrackPosition,
                    const DigitalStage::Api::Store*>
        customAudioTrackPositionAdded;
    sigslot::signal<const std::string&, nlohmann::json,
                    const DigitalStage::Api::Store*>
        customAudioTrackPositionChanged;
    sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
        customAudioTrackPositionRemoved;

    sigslot::signal<const CustomAudioTrackVolume,
                    const DigitalStage::Api::Store*>
        customAudioTrackVolumeAdded;
    sigslot::signal<const std::string&, nlohmann::json,
                    const DigitalStage::Api::Store*>
        customAudioTrackVolumeChanged;
    sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
        customAudioTrackVolumeRemoved;

    sigslot::signal<const User, const DigitalStage::Api::Store*> userAdded;
    sigslot::signal<const std::string&, nlohmann::json,
                    const DigitalStage::Api::Store*>
        userChanged;
    sigslot::signal<const std::string&, const DigitalStage::Api::Store*>
        userRemoved;

    /**
     * Not implemented, but could be an alternative to the store
     * @return
     */
    [[maybe_unused]] DigitalStage::Types::WholeStage getWholeStage() const;

    /**
     * Not implemented, but could be an alternative to the store
     */
    [[maybe_unused]] void setWholeStage(nlohmann::json wholeStage);

  private:
    const std::string apiUrl_;
    std::unique_ptr<Store> store_;
    nlohmann::json wholeStage_;
    mutable std::mutex wholeStage_mutex_;
    std::unique_ptr<teckos::client> wsclient_;
  };
} // namespace DigitalStage::Api

#endif // DS_CLIENT