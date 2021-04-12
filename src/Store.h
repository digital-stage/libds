//
// Created by Tobias Hegemann on 24.02.21.
//

#ifndef DS_STORE_H
#define DS_STORE_H

#include "Types.h"
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

using nlohmann::json;

namespace DigitalStage {
  class Store {
  public:
    Store();
    ~Store();

    void setLocalDevice(const json localDevice);

    void updateLocalDevice(const json update);

    std::optional<const DigitalStage::device_t> getLocalDevice();

    void setLocalUser(const json localUser);

    std::optional<const DigitalStage::user_t> getLocalUser();

    void createUser(const json user);

    void updateUser(const std::string& id, const json update);

    std::optional<const DigitalStage::user_t> readUser(const std::string& id);

    void removeUser(const std::string& id);

    void removeUsers();

    void createStage(const json stage);

    void updateStage(const std::string& stageId, const json update);

    std::optional<const DigitalStage::stage_t> readStage(const std::string& stageId);

    void removeStage(const std::string& stageId);

    void removeStages();

    void createGroup(const json group);

    void updateGroup(const std::string& id, const json update);

    std::optional<const DigitalStage::group_t> readGroup(const std::string& id);

    void removeGroup(const std::string& id);

    void removeGroups();

    void createCustomGroupPosition(const json customGroup);

    void updateCustomGroupPosition(const std::string& id, const json update);

    std::optional<const DigitalStage::custom_group_position_t>
    readCustomGroupPosition(const std::string& id);

    void removeCustomGroupPosition(const std::string& id);

    void removeCustomGroupPositions();

    void createCustomGroupVolume(const json customGroup);

    void updateCustomGroupVolume(const std::string& id, const json update);

    std::optional<const DigitalStage::custom_group_volume_t>
    readCustomGroupVolume(const std::string& id);

    void removeCustomGroupVolume(const std::string& id);

    void removeCustomGroupVolumes();

    void createStageMember(const json stageMember);

    void updateStageMember(const std::string& id, const json update);

    std::optional<const DigitalStage::stage_member_t>
    readStageMember(const std::string& id);

    const std::vector<DigitalStage::stage_member_t>
    readStageMembersByStage(const std::string& stageId);

    const std::vector<DigitalStage::stage_member_t>
    readStageMembersByGroup(const std::string& groupId);

    void removeStageMember(const std::string& id);

    void removeStageMembers();

    void createCustomStageMemberPosition(const json customStageMember);

    void updateCustomStageMemberPosition(const std::string& id,
                                         const json update);

    std::optional<const DigitalStage::custom_stage_member_position_t>
    readCustomStageMemberPosition(const std::string& id);

    std::optional<const DigitalStage::custom_stage_member_position_t>
    readCustomStageMemberPositionByStageMember(
        const std::string& stageMemberId);

    void removeCustomStageMemberPosition(const std::string& id);

    void removeCustomStageMemberPositions();

    void createCustomStageMemberVolume(const json customStageMember);

    void updateCustomStageMemberVolume(const std::string& id,
                                       const json update);

    std::optional<const DigitalStage::custom_stage_member_volume_t>
    readCustomStageMemberVolume(const std::string& id);

    std::optional<const DigitalStage::custom_stage_member_volume_t>
    readCustomStageMemberVolumeByStageMember(const std::string& stageMemberId);

    void removeCustomStageMemberVolume(const std::string& id);

    void removeCustomStageMemberVolumes();

    void createSoundCard(const json soundCard);

    void updateSoundCard(const std::string& id, const json update);

    std::optional<const DigitalStage::soundcard_t> readSoundCard(const std::string& id);

    std::optional<const DigitalStage::soundcard_t>
    readSoundCardByName(const std::string& name);

    void removeSoundCard(const std::string& id);

    void removeSoundCards();

    void createOvTrack(const json ovTrack);

    void updateOvTrack(const std::string& id, const json update);

    std::optional<const DigitalStage::ov_track_t> readOvTrack(const std::string& id);

    const std::vector<DigitalStage::ov_track_t> readOvTracks();

    void removeOvTrack(const std::string& id);

    void removeOvTracks();

    void createRemoteOvTrack(const json remoteOvTrack);

    void updateRemoteOvTrack(const std::string& id, const json update);

    std::optional<const DigitalStage::remote_ov_track_t>
    readRemoteOvTrack(const std::string& id);

    void removeRemoteOvTrack(const std::string& id);

    void removeRemoteOvTracks();

    void createCustomRemoteOvTrackPosition(const json customRemoteOvTrack);

    void updateCustomRemoteOvTrackPosition(const std::string& id,
                                           const json update);

    std::optional<const DigitalStage::custom_remote_ov_track_position_t>
    readCustomRemoteOvTrackPosition(const std::string& id);

    void removeCustomRemoteOvTrackPosition(const std::string& id);

    void removeCustomRemoteOvTrackPositions();

    void createCustomRemoteOvTrackVolume(const json customRemoteOvTrack);

    void updateCustomRemoteOvTrackVolume(const std::string& id,
                                         const json update);

    std::optional<const DigitalStage::custom_remote_ov_track_volume_t>
    readCustomRemoteOvTrackVolume(const std::string& id);

    void removeCustomRemoteOvTrackVolume(const std::string& id);

    void removeCustomRemoteOvTrackVolumes();

    /* CUSTOMIZED METHODS */

    const std::string& getCurrentStageId();

    void setCurrentStageId(const std::string& stageId);

    std::optional<const DigitalStage::stage_t> getCurrentStage();

    std::optional<const DigitalStage::stage_member_t> getCurrentStageMember();

    std::optional<const DigitalStage::custom_stage_member_position_t>
    getCustomStageMemberPositionByStageMemberId(
        const std::string& stageMemberId);

    std::optional<const DigitalStage::custom_stage_member_volume_t>
    getCustomStageMemberVolumeByStageMemberId(const std::string& stageMemberId);

    std::optional<const DigitalStage::custom_group_position_t>
    getCustomGroupPositionByGroupId(const std::string& customStageMemberId);

    std::optional<const DigitalStage::custom_group_volume_t>
    getCustomGroupVolumeByGroupId(const std::string& customStageMemberId);

    std::optional<const DigitalStage::custom_remote_ov_track_position_t>
    getCustomOvTrackPositionByOvTrackId(const std::string& ovTrackId);

    std::optional<const DigitalStage::custom_remote_ov_track_volume_t>
    getCustomOvTrackVolumeByOvTrackId(const std::string& ovTrackId);

    const std::vector<DigitalStage::remote_ov_track_t>
    getRemoteOvTracksByStageMemberId(const std::string& stageMemberId);

    void dump();

  private:
    std::recursive_mutex local_device_mutex_;
    std::recursive_mutex local_user_mutex_;
    std::recursive_mutex users_mutex_;
    std::recursive_mutex stages_mutex_;
    std::recursive_mutex groups_mutex_;
    std::recursive_mutex custom_group_positions_mutex_;
    std::recursive_mutex custom_group_volumes_mutex_;
    std::recursive_mutex stage_members_mutex_;
    std::recursive_mutex custom_stage_member_positions_mutex_;
    std::recursive_mutex custom_stage_member_volumes_mutex_;
    std::recursive_mutex sound_cards_mutex_;
    std::recursive_mutex ov_tracks_mutex_;
    std::recursive_mutex remote_ov_tracks_mutex_;
    std::recursive_mutex custom_remote_ov_track_positions_mutex_;
    std::recursive_mutex custom_remote_ov_track_volumes_mutex_;
    std::recursive_mutex current_stage_id_mutex_;

    // Internal data store
    std::map<std::string, json> customRemoteOvTrackPositions_;
    std::map<std::string, json> customRemoteOvTrackVolumes_;
    std::map<std::string, json> remoteOvTracks_;
    std::map<std::string, json> customStageMemberTrackPositions_;
    std::map<std::string, json> customStageMemberTrackVolumes_;
    std::map<std::string, json> stageMemberTracks_;
    std::map<std::string, json> customStageMemberPositions_;
    std::map<std::string, json> customStageMemberVolumes_;
    std::map<std::string, json> stageMembers_;
    std::map<std::string, json> customGroupPositions_;
    std::map<std::string, json> customGroupVolumes_;
    std::map<std::string, json> groups_;
    std::map<std::string, json> stages_;
    std::map<std::string, json> ovTracks_;
    std::map<std::string, json> soundCards_;
    std::map<std::string, json> users_;
    json localDevice_;
    json localUser_;
    std::string currentStageId_;
    std::string currentStageMemberId_;

    // Helper data stores
    std::map<std::string, std::string> customGroupPositionIdByGroupId_;
    std::map<std::string, std::string> customGroupVolumeIdByGroupId_;
    std::map<std::string, std::vector<std::string>> stageMemberIdsByStageId_;
    std::map<std::string, std::vector<std::string>> stageMemberIdsByGroupId_;
    std::map<std::string, std::vector<std::string>>
        remoteOvTrackIdsByStageMemberId_;
    std::map<std::string, std::string>
        customStageMemberVolumeIdByStageMemberId_;
    std::map<std::string, std::string>
        customStageMemberPositionIdByStageMemberId_;
    std::map<std::string, std::string> customOvTrackPositionIdByOvTrackId_;
    std::map<std::string, std::string> customOvTrackVolumeIdByOvTrackId_;
    std::map<std::string, std::string> soundCardIdByName_;
  };
} // namespace ds

#endif // DS_STORE_H