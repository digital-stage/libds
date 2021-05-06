
#ifndef DS_STORE
#define DS_STORE

#include "DigitalStage/Types.h"
#include "Macros.h"
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <set>

using namespace nlohmann;

using namespace DigitalStage::Types;

namespace DigitalStage::Api {
  class Store {
  public:
    Store();

    void setReady(bool ready);
    bool isReady() const;

    // Local device
    std::optional<const Device> getLocalDevice() const;
    void setLocalDeviceId(const std::string& id);
    std::optional<std::string> getLocalDeviceId() const;

    // Local user
    void setUserId(const std::string& id);
    std::optional<std::string> getUserId() const;
    void setStageMemberId(const std::string& id);
    [[maybe_unused]] std::optional<std::string> getStageMemberId() const;

    // Stage management
    void setStageId(const std::string& id);
    void resetStageId();
    std::optional<std::string> getStageId() const;

    void setGroupId(const std::string& id);
    void resetGroupId();
    [[maybe_unused]] std::optional<std::string> getGroupId() const;

    void setStageDeviceId(const std::string& id);
    void resetStageDeviceId();
    std::optional<std::string> getStageDeviceId() const;
    std::optional<DigitalStage::Types::StageDevice> getStageDevice() const;

    // Groups
    STORE_GET(DigitalStage::Types::Group, Group, groups_mutex_, groups_)
    STORE_GET_ALL(DigitalStage::Types::Group, Group, groups_mutex_, groups_)
    STORE_REMOVE_ALL(DigitalStage::Types::Group, Group, groups_mutex_, groups_)
    std::vector<Group> getGroupsByStage(const std::string& stageId) const;
    void createGroup(const json& payload);
    STORE_UPDATE(DigitalStage::Types::Group, Group, groups_mutex_, groups_)
    void removeGroup(const std::string& id);

    // Stage members
    STORE_GET(DigitalStage::Types::StageMember, StageMember,
              stageMembers_mutex_, stageMembers_)
    STORE_GET_ALL(DigitalStage::Types::StageMember, StageMember,
                  stageMembers_mutex_, stageMembers_)
    STORE_REMOVE_ALL(DigitalStage::Types::StageMember, StageMember,
                     stageMembers_mutex_, stageMembers_)
    [[maybe_unused]] std::vector<DigitalStage::Types::StageMember>
    getStageMembersByStage(const std::string& stageId) const;
    std::vector<DigitalStage::Types::StageMember>
    getStageMembersByGroup(const std::string& audio_trackId) const;
    void createStageMember(const json& payload);
    STORE_UPDATE(StageMember, StageMember, stageMembers_mutex_, stageMembers_)
    void removeStageMember(const std::string& id);

    // Stage devices
    STORE_GET(DigitalStage::Types::StageDevice, StageDevice,
              stageDevices_mutex_, stageDevices_)
    STORE_GET_ALL(DigitalStage::Types::StageDevice, StageDevice,
                  stageDevices_mutex_, stageDevices_)
    STORE_REMOVE_ALL(DigitalStage::Types::StageDevice, StageDevice,
                     stageDevices_mutex_, stageDevices_)
    std::vector<DigitalStage::Types::StageDevice>
    getStageDevicesByStageMember(const std::string& stageMemberId) const;
    void createStageDevice(const json& payload);
    STORE_UPDATE(StageDevice, StageDevice, stageDevices_mutex_, stageDevices_)
    void removeStageDevice(const std::string& id);

    // Video tracks
    STORE_GET(DigitalStage::Types::VideoTrack, VideoTrack,
              videoTracks_mutex_, videoTracks_)
    STORE_GET_ALL(DigitalStage::Types::VideoTrack, VideoTrack,
                  videoTracks_mutex_, videoTracks_)
    STORE_UPDATE(DigitalStage::Types::VideoTrack, VideoTrack,
                 videoTracks_mutex_, videoTracks_)
    STORE_REMOVE_ALL(DigitalStage::Types::VideoTrack,
                     VideoTrack, videoTracks_mutex_,
                     videoTracks_)
    std::vector<DigitalStage::Types::VideoTrack>
    getVideoTracksByStageDevice(const std::string& stageDeviceId) const;
    void createVideoTrack(const json& payload);
    void removeVideoTrack(const std::string& id);

    // Audio tracks
    STORE_GET(DigitalStage::Types::AudioTrack, AudioTrack,
              audioTracks_mutex_, audioTracks_)
    STORE_GET_ALL(DigitalStage::Types::AudioTrack, AudioTrack,
                  audioTracks_mutex_, audioTracks_)
    STORE_UPDATE(DigitalStage::Types::AudioTrack, AudioTrack,
                 audioTracks_mutex_, audioTracks_)
    STORE_REMOVE_ALL(DigitalStage::Types::AudioTrack,
                     AudioTrack, audioTracks_mutex_,
                     audioTracks_)
    std::vector<AudioTrack>
    getAudioTracksByStageDevice(const std::string& stageDeviceId) const;
    void createAudioTrack(const json& payload);
    void removeAudioTrack(const std::string& id);

    // Custom group positions
    STORE_GET(DigitalStage::Types::CustomGroupPosition, CustomGroupPosition,
              customGroupPositions_mutex_, customGroupPositions_)
    STORE_GET_ALL(DigitalStage::Types::CustomGroupPosition, CustomGroupPosition,
                  customGroupPositions_mutex_, customGroupPositions_)
    STORE_REMOVE_ALL(DigitalStage::Types::CustomGroupPosition,
                     CustomGroupPosition, customGroupPositions_mutex_,
                     customGroupPositions_)
    std::optional<const CustomGroupPosition>
    getCustomGroupPositionByGroupAndDevice(const std::string& groupId,
                                           const std::string& deviceId) const;
    void createCustomGroupPosition(const json& payload);
    STORE_UPDATE(CustomGroupPosition, CustomGroupPosition,
                 customGroupPositions_mutex_, customGroupPositions_)
    void removeCustomGroupPosition(const std::string& id);

    // Custom group volumes
    STORE_GET(DigitalStage::Types::CustomGroupVolume, CustomGroupVolume,
              customGroupVolumes_mutex_, customGroupVolumes_)
    STORE_GET_ALL(DigitalStage::Types::CustomGroupVolume, CustomGroupVolume,
                  customGroupVolumes_mutex_, customGroupVolumes_)
    STORE_UPDATE(DigitalStage::Types::CustomGroupVolume, CustomGroupVolume,
                 customGroupVolumes_mutex_, customGroupVolumes_)
    STORE_REMOVE_ALL(CustomGroupVolume, CustomGroupVolume,
                     customGroupVolumes_mutex_, customGroupVolumes_)
    std::optional<const CustomGroupVolume>
    getCustomGroupVolumeByGroupAndDevice(const std::string& groupId,
                                         const std::string& deviceId) const;
    void createCustomGroupVolume(const json& payload);
    void removeCustomGroupVolume(const std::string& id);

    // Custom stage member positions
    STORE_GET(CustomStageMemberPosition, CustomStageMemberPosition,
              customStageMemberPositions_mutex_, customStageMemberPositions_)
    STORE_GET_ALL(CustomStageMemberPosition, CustomStageMemberPosition,
                  customStageMemberPositions_mutex_,
                  customStageMemberPositions_)
    STORE_REMOVE_ALL(custom_stage_member_position_t, CustomStageMemberPosition,
                     customStageMemberPositions_mutex_,
                     customStageMemberPositions_)
    [[maybe_unused]] std::optional<const CustomStageMemberPosition>
    getCustomStageMemberPositionByStageMemberAndDevice(
        const std::string& stageMemberId, const std::string& deviceId) const;
    void createCustomStageMemberPosition(const json& payload);
    STORE_UPDATE(custom_stage_member_position_t, CustomStageMemberPosition,
                 customStageMemberPositions_mutex_, customStageMemberPositions_)
    void removeCustomStageMemberPosition(const std::string& id);

    // Custom stage member volumes
    STORE_GET(CustomStageMemberVolume, CustomStageMemberVolume,
              customStageMemberVolumes_mutex_, customStageMemberVolumes_)
    STORE_GET_ALL(CustomStageMemberVolume, CustomStageMemberVolume,
                  customStageMemberVolumes_mutex_, customStageMemberVolumes_)
    STORE_UPDATE(custom_stage_member_volume_t, CustomStageMemberVolume,
                 customStageMemberVolumes_mutex_, customStageMemberVolumes_)
    STORE_REMOVE_ALL(custom_stage_member_volume_t, CustomStageMemberVolume,
                     customStageMemberVolumes_mutex_, customStageMemberVolumes_)
    [[maybe_unused]] std::optional<const CustomStageMemberVolume>
    getCustomStageMemberVolumeByStageMemberAndDevice(
        const std::string& stageMemberId, const std::string& deviceId) const;
    void createCustomStageMemberVolume(const json& payload);
    void removeCustomStageMemberVolume(const std::string& id);

    // Custom stage device positions
    STORE_GET(CustomStageDevicePosition, CustomStageDevicePosition,
              customStageDevicePositions_mutex_, customStageDevicePositions_)
    STORE_GET_ALL(CustomStageDevicePosition, CustomStageDevicePosition,
                  customStageDevicePositions_mutex_,
                  customStageDevicePositions_)
    STORE_REMOVE_ALL(CustomStageDevicePosition, CustomStageDevicePosition,
                     customStageDevicePositions_mutex_,
                     customStageDevicePositions_)
    [[maybe_unused]] std::optional<const CustomStageDevicePosition>
    getCustomStageDevicePositionByStageDeviceAndDevice(
        const std::string& stageDeviceId, const std::string& deviceId) const;
    void createCustomStageDevicePosition(const json& payload);
    STORE_UPDATE(CustomStageDevicePosition, CustomStageDevicePosition,
                 customStageDevicePositions_mutex_, customStageDevicePositions_)
    void removeCustomStageDevicePosition(const std::string& id);

    // Custom stage device volumes
    STORE_GET(CustomStageDeviceVolume, CustomStageDeviceVolume,
              customStageDeviceVolumes_mutex_, customStageDeviceVolumes_)
    STORE_GET_ALL(CustomStageDeviceVolume, CustomStageDeviceVolume,
                  customStageDeviceVolumes_mutex_, customStageDeviceVolumes_)
    STORE_UPDATE(CustomStageDeviceVolume, CustomStageDeviceVolume,
                 customStageDeviceVolumes_mutex_, customStageDeviceVolumes_)
    STORE_REMOVE_ALL(CustomStageDeviceVolume, CustomStageDeviceVolume,
                     customStageDeviceVolumes_mutex_, customStageDeviceVolumes_)
    [[maybe_unused]] std::optional<const CustomStageDeviceVolume>
    getCustomStageDeviceVolumeByStageDeviceAndDevice(
        const std::string& stageDeviceId, const std::string& deviceId) const;
    void createCustomStageDeviceVolume(const json& payload);
    void removeCustomStageDeviceVolume(const std::string& id);

    // Custom audio track positions
    STORE_GET(CustomAudioTrackPosition,
              CustomAudioTrackPosition,
              customAudioTrackPositions_mutex_,
              customAudioTrackPositions_)
    STORE_GET_ALL(CustomAudioTrackPosition,
                  CustomAudioTrackPosition,
                  customAudioTrackPositions_mutex_,
                  customAudioTrackPositions_)
    STORE_REMOVE_ALL(custom_audio_track_position_t,
                     CustomAudioTrackPosition,
                     customAudioTrackPositions_mutex_,
                     customAudioTrackPositions_)
    [[maybe_unused]] std::optional<const CustomAudioTrackPosition>
    getCustomAudioTrackPositionByAudioTrackAndDevice(
        const std::string& audio_trackId,
        const std::string& deviceId) const;
    void createCustomAudioTrackPosition(const json& payload);
    STORE_UPDATE(custom_audio_track_position_t,
                 CustomAudioTrackPosition,
                 customAudioTrackPositions_mutex_,
                 customAudioTrackPositions_)
    void removeCustomAudioTrackPosition(const std::string& id);

    // Custom audio track volumes
    STORE_GET(CustomAudioTrackVolume, CustomAudioTrackVolume,
              customAudioTrackVolumes_mutex_,
              customAudioTrackVolumes_)
    STORE_GET_ALL(CustomAudioTrackVolume,
                  CustomAudioTrackVolume,
                  customAudioTrackVolumes_mutex_,
                  customAudioTrackVolumes_)
    STORE_UPDATE(custom_audio_track_volume_t,
                 CustomAudioTrackVolume,
                 customAudioTrackVolumes_mutex_,
                 customAudioTrackVolumes_)
    STORE_REMOVE_ALL(custom_audio_track_volume_t,
                     CustomAudioTrackVolume,
                     customAudioTrackVolumes_mutex_,
                     customAudioTrackVolumes_)
    [[maybe_unused]] std::optional<const CustomAudioTrackVolume>
    getCustomAudioTrackVolumeByAudioTrackAndDevice(
        const std::string& audio_trackId,
        const std::string& deviceId) const;
    void createCustomAudioTrackVolume(const json& payload);
    void removeCustomAudioTrackVolume(const std::string& id);

    // Sound cards
    std::optional<DigitalStage::Types::SoundCard>
    getSoundCardByUUID(const std::string& uuid) const;

    // Devices
    ADD_STORE_ENTRY(DigitalStage::Types::Device, Device, devices_)

    // Sound cards
    ADD_STORE_ENTRY(DigitalStage::Types::SoundCard, SoundCard, soundCards_)

    // Stages
    ADD_STORE_ENTRY(DigitalStage::Types::Stage, Stage, stages_)

    // Users
    ADD_STORE_ENTRY(DigitalStage::Types::User, User, users_)

  protected:
    mutable std::recursive_mutex ready_mutex_;
    bool isReady_;

    mutable std::recursive_mutex userId_mutex_;
    std::optional<std::string> userId_;

    mutable std::recursive_mutex stageId_mutex_;
    std::optional<std::string> stageId_;
    mutable std::recursive_mutex stageMemberId_mutex_;
    std::optional<std::string> stageMemberId_;
    mutable std::recursive_mutex groupId_mutex_;
    std::optional<std::string> groupId_;

    mutable std::recursive_mutex local_device_id_mutex_;
    std::optional<std::string> localDeviceId_;

    mutable std::recursive_mutex stage_device_id_mutex_;
    std::optional<std::string> stageDeviceId_;

    mutable std::recursive_mutex groups_mutex_;
    std::map<std::string, json> groups_;
    std::map<std::string, std::set<std::string>> groupIds_by_stages_;

    mutable std::recursive_mutex stageMembers_mutex_;
    std::map<std::string, json> stageMembers_;
    std::map<std::string, std::set<std::string>> stageMemberIds_by_stages_;
    std::map<std::string, std::set<std::string>> stageMemberIds_by_groups_;

    mutable std::recursive_mutex stageDevices_mutex_;
    std::map<std::string, json> stageDevices_;
    std::map<std::string, std::set<std::string>> stageDeviceIds_by_stageMember_;

    mutable std::recursive_mutex customGroupVolumes_mutex_;
    std::map<std::string, json> customGroupVolumes_;
    std::map<std::string, std::map<std::string, std::string>>
        customGroupVolumeIds_by_Group_and_Device_;

    mutable std::recursive_mutex customGroupPositions_mutex_;
    std::map<std::string, json> customGroupPositions_;
    std::map<std::string, std::map<std::string, std::string>>
        customGroupPositionIds_by_Group_and_Device_;

    mutable std::recursive_mutex customStageMemberPositions_mutex_;
    std::map<std::string, json> customStageMemberPositions_;
    std::map<std::string, std::map<std::string, std::string>>
        customStageMemberPositionIds_by_StageMember_and_Device_;

    mutable std::recursive_mutex customStageMemberVolumes_mutex_;
    std::map<std::string, json> customStageMemberVolumes_;
    std::map<std::string, std::map<std::string, std::string>>
        customStageMemberVolumeIds_by_StageMember_and_Device_;

    mutable std::recursive_mutex customStageDevicePositions_mutex_;
    std::map<std::string, json> customStageDevicePositions_;
    std::map<std::string, std::map<std::string, std::string>>
        customStageDevicePositionIds_by_StageDevice_and_Device_;

    mutable std::recursive_mutex customStageDeviceVolumes_mutex_;
    std::map<std::string, json> customStageDeviceVolumes_;
    std::map<std::string, std::map<std::string, std::string>>
        customStageDeviceVolumeIds_by_StageDevice_and_Device_;

    mutable std::recursive_mutex videoTracks_mutex_;
    std::map<std::string, json> videoTracks_;
    std::map<std::string, std::set<std::string>>
        videoTrackIds_by_StageDevice_;

    mutable std::recursive_mutex audioTracks_mutex_;
    std::map<std::string, json> audioTracks_;
    std::map<std::string, std::set<std::string>>
        audioTrackIds_by_StageDevice_;

    mutable std::recursive_mutex customAudioTrackVolumes_mutex_;
    std::map<std::string, json> customAudioTrackVolumes_;
    std::map<std::string, std::map<std::string, std::string>>
        customAudioTrackVolumeIds_by_AudioTrack_and_Device_;

    mutable std::recursive_mutex customAudioTrackPositions_mutex_;
    std::map<std::string, json> customAudioTrackPositions_;
    std::map<std::string, std::map<std::string, std::string>>
        customAudioTrackPositionIds_by_AudioTrack_and_Device_;
  };
} // namespace DigitalStage::Api

#endif // DS_STORE