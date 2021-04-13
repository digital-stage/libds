#include "Store.h"
#include <iostream>

DigitalStage::Store::Store() {}

DigitalStage::Store::~Store() {}

void DigitalStage::Store::createStage(const nlohmann::json stage)
{
  std::lock_guard<std::recursive_mutex>(this->stages_mutex_);
  const std::string _id = stage.at("_id").get<std::string>();
  this->stages_[_id] = stage;
}

void DigitalStage::Store::updateStage(const std::string& stageId,
                                      const nlohmann::json update)
{
  std::lock_guard<std::recursive_mutex>(this->stages_mutex_);
  this->stages_[stageId].merge_patch(update);
}

std::optional<const DigitalStage::stage_t>
DigitalStage::Store::readStage(const std::string& stageId)
{
  std::lock_guard<std::recursive_mutex>(this->stages_mutex_);
  if(this->stages_.count(stageId) > 0)
    return this->stages_[stageId].get<DigitalStage::stage_t>();
  return std::nullopt;
}

void DigitalStage::Store::removeStage(const std::string& stageId)
{
  std::lock_guard<std::recursive_mutex>(this->stages_mutex_);
  this->stages_.erase(stageId);
}

void DigitalStage::Store::createGroup(const nlohmann::json group)
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  const std::string _id = group.at("_id").get<std::string>();
  this->groups_[_id] = group;
}

void DigitalStage::Store::updateGroup(const std::string& id,
                                      const nlohmann::json update)
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  this->groups_[id].merge_patch(update);
}

std::optional<const DigitalStage::group_t>
DigitalStage::Store::readGroup(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  if(this->groups_.count(id) > 0)
    return this->groups_[id].get<DigitalStage::group_t>();
  return std::nullopt;
}

void DigitalStage::Store::removeGroup(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  this->groups_.erase(id);
}

const std::string& DigitalStage::Store::getCurrentStageId()
{
  std::lock_guard<std::recursive_mutex>(this->current_stage_id_mutex_);
  return this->currentStageId_;
}

void DigitalStage::Store::setCurrentStageId(const std::string& stageId)
{
  std::lock_guard<std::recursive_mutex>(this->current_stage_id_mutex_);
  this->currentStageId_ = stageId;
  // Also update current stage member
}

void DigitalStage::Store::setLocalDevice(const nlohmann::json localDevice)
{
  std::lock_guard<std::recursive_mutex>(this->local_device_mutex_);
  this->localDevice_ = localDevice;
}

std::optional<const DigitalStage::device_t>
DigitalStage::Store::getLocalDevice()
{
  std::lock_guard<std::recursive_mutex>(this->local_device_mutex_);
  if(!(this->.is_null())) {
    return this->localDevice_.get<DigitalStage::device_t>();
  }
  return std::nullopt;
}

void DigitalStage::Store::updateLocalDevice(const nlohmann::json update)
{
  std::lock_guard<std::recursive_mutex>(this->local_device_mutex_);
  this->localDevice_.merge_patch(update);
}

void DigitalStage::Store::setLocalUser(const nlohmann::json localUser)
{
  std::lock_guard<std::recursive_mutex>(this->local_user_mutex_);
  this->localUser_ = localUser;
}

std::optional<const DigitalStage::user_t> DigitalStage::Store::getLocalUser()
{
  std::lock_guard<std::recursive_mutex>(this->local_user_mutex_);
  if(!(this->localUser_.is_null()))
    return this->localUser_.get<DigitalStage::user_t>();
  return std::nullopt;
}

void DigitalStage::Store::removeStages()
{
  std::lock_guard<std::recursive_mutex>(this->stages_mutex_);
  this->stages_.clear();
}

void DigitalStage::Store::removeGroups()
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  this->groups_.clear();
}

void DigitalStage::Store::createCustomGroupPosition(
    const nlohmann::json customGroup)
{
  std::lock_guard<std::recursive_mutex>(this->custom_group_positions_mutex_);
  const std::string _id = customGroup.at("_id").get<std::string>();
  this->customGroupPositions_[_id] = customGroup;
  this->customGroupPositionIdByGroupId_[customGroup["groupId"]] = _id;
}

void DigitalStage::Store::updateCustomGroupPosition(const std::string& id,
                                                    const nlohmann::json update)
{
  std::lock_guard<std::recursive_mutex>(this->custom_group_positions_mutex_);
  this->customGroupPositions_[id].merge_patch(update);
}

std::optional<const DigitalStage::custom_group_position_t>
DigitalStage::Store::readCustomGroupPosition(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->custom_group_positions_mutex_);
  if(this->customGroupPositions_.count(id) > 0)
    return this->customGroupPositions_[id]
        .get<DigitalStage::custom_group_position_t>();
  return std::nullopt;
}

void DigitalStage::Store::removeCustomGroupPosition(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->custom_group_positions_mutex_);
  const std::string groupId =
      this->customGroupPositions_[id].at("groupId").get<std::string>();
  this->customGroupPositions_.erase(id);
  this->customGroupPositionIdByGroupId_.erase(groupId);
}

void DigitalStage::Store::removeCustomGroupPositions()
{
  std::lock_guard<std::recursive_mutex>(this->custom_group_positions_mutex_);
  this->customGroupPositions_.clear();
  this->customGroupPositionIdByGroupId_.clear();
}

void DigitalStage::Store::createCustomGroupVolume(
    const nlohmann::json customGroup)
{
  std::lock_guard<std::recursive_mutex>(this->custom_group_volumes_mutex_);
  const std::string _id = customGroup.at("_id").get<std::string>();
  this->customGroupVolumes_[_id] = customGroup;
  this->customGroupVolumeIdByGroupId_[customGroup["groupId"]] = _id;
}

void DigitalStage::Store::updateCustomGroupVolume(const std::string& id,
                                                  const nlohmann::json update)
{
  std::lock_guard<std::recursive_mutex>(this->custom_group_volumes_mutex_);
  this->customGroupVolumes_[id].merge_patch(update);
}

std::optional<const DigitalStage::custom_group_volume_t>
DigitalStage::Store::readCustomGroupVolume(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->custom_group_volumes_mutex_);
  if(this->customGroupVolumes_.count(id) > 0)
    return this->customGroupVolumes_[id]
        .get<DigitalStage::custom_group_volume_t>();
  return std::nullopt;
}

void DigitalStage::Store::removeCustomGroupVolume(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->custom_group_volumes_mutex_);
  const std::string groupId =
      this->customGroupVolumes_[id].at("groupId").get<std::string>();
  this->customGroupVolumes_.erase(id);
  this->customGroupVolumeIdByGroupId_.erase(groupId);
}

void DigitalStage::Store::removeCustomGroupVolumes()
{
  std::lock_guard<std::recursive_mutex>(this->custom_group_volumes_mutex_);
  this->customGroupVolumes_.clear();
  this->customGroupVolumeIdByGroupId_.clear();
}

void DigitalStage::Store::createStageMember(const nlohmann::json stageMember)
{
  std::lock_guard<std::recursive_mutex>(this->stage_members_mutex_);
  const std::string _id = stageMember.at("_id").get<std::string>();
  const std::string stageId = stageMember.at("stageId").get<std::string>();
  const std::string groupId = stageMember.at("groupId").get<std::string>();
  this->stageMembers_[_id] = stageMember;
  this->stageMemberIdsByStageId_[stageId].push_back(_id);
  this->stageMemberIdsByStageId_[groupId].push_back(_id);
  const auto localUser = this->getLocalUser();
  if(localUser && localUser->_id == stageMember["userId"]) {
    this->currentStageMemberId_ = _id;
  }
}

void DigitalStage::Store::updateStageMember(const std::string& id,
                                            const nlohmann::json update)
{
  std::lock_guard<std::recursive_mutex>(this->stage_members_mutex_);
  this->stageMembers_[id].merge_patch(update);
}

std::optional<const DigitalStage::stage_member_t>
DigitalStage::Store::readStageMember(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->stage_members_mutex_);
  if(this->stageMembers_.count(id) > 0) {
    return this->stageMembers_[id].get<DigitalStage::stage_member_t>();
  }
  return std::nullopt;
}

const std::vector<DigitalStage::stage_member_t>
DigitalStage::Store::readStageMembersByStage(const std::string& stageId)
{
  std::lock_guard<std::recursive_mutex>(this->stage_members_mutex_);
  std::vector<DigitalStage::stage_member_t> stageMembers;
  if(this->stageMemberIdsByStageId_.count(stageId) > 0) {
    for(const auto& stageMemberId : this->stageMemberIdsByStageId_[stageId]) {
      const auto stageMember = this->readStageMember(stageMemberId);
      if(stageMember) {
        stageMembers.push_back(*stageMember);
      }
    }
  }
  return stageMembers;
}

const std::vector<DigitalStage::stage_member_t>
DigitalStage::Store::readStageMembersByGroup(const std::string& groupId)
{
  std::lock_guard<std::recursive_mutex>(this->stage_members_mutex_);
  std::vector<DigitalStage::stage_member_t> stageMembers;
  if(this->stageMemberIdsByGroupId_.count(groupId) > 0) {
    for(const auto& stageMemberId : this->stageMemberIdsByGroupId_[groupId]) {
      const auto stageMember = this->readStageMember(stageMemberId);
      if(stageMember) {
        stageMembers.push_back(*stageMember);
      }
    }
  }
  return stageMembers;
}

void DigitalStage::Store::removeStageMember(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->stage_members_mutex_);
  const std::string stageId =
      this->stageMembers_[id].at("stageId").get<std::string>();
  const std::string groupId =
      this->stageMembers_[id].at("groupId").get<std::string>();
  const std::string userId =
      this->stageMembers_[id].at("userId").get<std::string>();
  this->stageMembers_.erase(id);
  std::remove(this->stageMemberIdsByStageId_[stageId].begin(),
              this->stageMemberIdsByStageId_[stageId].end(), id);
  std::remove(this->stageMemberIdsByGroupId_[groupId].begin(),
              this->stageMemberIdsByGroupId_[groupId].end(), id);
  const auto localUser = this->getLocalUser();
  if(localUser && localUser->_id == userId) {
    this->currentStageMemberId_.clear();
  }
}

void DigitalStage::Store::removeStageMembers()
{
  std::lock_guard<std::recursive_mutex>(this->stage_members_mutex_);
  this->stageMembers_.clear();
  this->stageMemberIdsByStageId_.clear();
}

void DigitalStage::Store::createCustomStageMemberPosition(
    const nlohmann::json customStageMember)
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_positions_mutex_);
  std::string _id = customStageMember.at("_id").get<std::string>();
  std::string stageMemberId =
      customStageMember["stageMemberId"].get<std::string>();
  this->customStageMemberPositions_[_id] = customStageMember;
  this->customStageMemberPositionIdByStageMemberId_[stageMemberId] = _id;
}

void DigitalStage::Store::updateCustomStageMemberPosition(
    const std::string& id, const nlohmann::json update)
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_positions_mutex_);
  this->customStageMemberPositions_[id].merge_patch(update);
  // We may implement the change of the stageMemberId, but this should never
  // happen if backend works as expected
}

std::optional<const DigitalStage::custom_stage_member_position_t>
DigitalStage::Store::readCustomStageMemberPosition(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_positions_mutex_);
  if(this->customStageMemberPositions_.count(id) > 0) {
    const DigitalStage::custom_stage_member_position_t customStageMember =
        this->customStageMemberPositions_[id]
            .get<DigitalStage::custom_stage_member_position_t>();
    return customStageMember;
  }
  return std::nullopt;
}

std::optional<const DigitalStage::custom_stage_member_position_t>
DigitalStage::Store::readCustomStageMemberPositionByStageMember(
    const std::string& stageMemberId)
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_positions_mutex_);
  if(this->customStageMemberPositionIdByStageMemberId_.count(stageMemberId)) {
    return this->readCustomStageMemberPosition(
        this->customStageMemberPositionIdByStageMemberId_[stageMemberId]);
  }
  return std::nullopt;
}

void DigitalStage::Store::removeCustomStageMemberPosition(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_positions_mutex_);
  const std::string stageMemberId =
      this->customStageMemberPositions_.at("stageMemberId").get<std::string>();
  this->customStageMemberPositions_.erase(id);
  this->customStageMemberPositionIdByStageMemberId_.erase(stageMemberId);
}

void DigitalStage::Store::removeCustomStageMemberPositions()
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_positions_mutex_);
  this->customStageMemberPositions_.clear();
  this->customStageMemberPositionIdByStageMemberId_.clear();
}

void DigitalStage::Store::createCustomStageMemberVolume(
    const nlohmann::json customStageMember)
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_volumes_mutex_);
  std::string _id = customStageMember.at("_id").get<std::string>();
  std::string stageMemberId =
      customStageMember["stageMemberId"].get<std::string>();
  this->customStageMemberVolumes_[_id] = customStageMember;
  this->customStageMemberVolumeIdByStageMemberId_[stageMemberId] = _id;
}

void DigitalStage::Store::updateCustomStageMemberVolume(
    const std::string& id, const nlohmann::json update)
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_volumes_mutex_);
  this->customStageMemberVolumes_[id].merge_patch(update);
  // We may implement the change of the stageMemberId, but this should never
  // happen if backend works as expected
}

std::optional<const DigitalStage::custom_stage_member_volume_t>
DigitalStage::Store::readCustomStageMemberVolume(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_volumes_mutex_);
  if(this->customStageMemberVolumes_.count(id) > 0) {
    const DigitalStage::custom_stage_member_volume_t customStageMember =
        this->customStageMemberVolumes_[id]
            .get<DigitalStage::custom_stage_member_volume_t>();
    return customStageMember;
  }
  return std::nullopt;
}

std::optional<const DigitalStage::custom_stage_member_volume_t>
DigitalStage::Store::readCustomStageMemberVolumeByStageMember(
    const std::string& stageMemberId)
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_volumes_mutex_);
  if(this->customStageMemberVolumeIdByStageMemberId_.count(stageMemberId)) {
    return this->readCustomStageMemberVolume(
        this->customStageMemberVolumeIdByStageMemberId_[stageMemberId]);
  }
  return std::nullopt;
}

void DigitalStage::Store::removeCustomStageMemberVolume(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_volumes_mutex_);
  const std::string stageMemberId =
      this->customStageMemberVolumes_.at("stageMemberId").get<std::string>();
  this->customStageMemberVolumes_.erase(id);
  this->customStageMemberVolumeIdByStageMemberId_.erase(stageMemberId);
}

void DigitalStage::Store::removeCustomStageMemberVolumes()
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_volumes_mutex_);
  this->customStageMemberVolumes_.clear();
  this->customStageMemberVolumeIdByStageMemberId_.clear();
}

void DigitalStage::Store::createSoundCard(const nlohmann::json soundCard)
{
  std::lock_guard<std::recursive_mutex>(this->sound_cards_mutex_);
  const std::string _id = soundCard.at("_id").get<std::string>();
  this->soundCards_[_id] = soundCard;
  this->soundCardIdByName_[soundCard["name"]] = _id;
}

void DigitalStage::Store::updateSoundCard(const std::string& id,
                                          const nlohmann::json update)
{
  std::lock_guard<std::recursive_mutex>(this->sound_cards_mutex_);
  this->soundCards_[id].merge_patch(update);
}

std::optional<const DigitalStage::soundcard_t>
DigitalStage::Store::readSoundCard(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->sound_cards_mutex_);
  if(this->soundCards_.count(id) > 0)
    return this->soundCards_[id].get<DigitalStage::soundcard_t>();
  return std::nullopt;
}

std::optional<const DigitalStage::soundcard_t>
DigitalStage::Store::readSoundCardByName(const std::string& name)
{
  std::lock_guard<std::recursive_mutex>(this->sound_cards_mutex_);
  if(this->soundCardIdByName_.count(name) > 0)
    return this->readSoundCard(this->soundCardIdByName_[name]);
  return std::nullopt;
}

void DigitalStage::Store::removeSoundCard(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->sound_cards_mutex_);
  const std::string soundCardName =
      this->soundCards_[id].at("name").get<std::string>();
  this->soundCards_.erase(id);
  this->soundCardIdByName_.erase(soundCardName);
}

void DigitalStage::Store::removeSoundCards()
{
  std::lock_guard<std::recursive_mutex>(this->sound_cards_mutex_);
  this->soundCards_.clear();
  this->soundCardIdByName_.clear();
}

void DigitalStage::Store::createOvTrack(const nlohmann::json ovTrack)
{
  std::lock_guard<std::recursive_mutex>(this->ov_tracks_mutex_);
  const std::string _id = ovTrack.at("_id").get<std::string>();
  this->ovTracks_[_id] = ovTrack;
}

void DigitalStage::Store::updateOvTrack(const std::string& id,
                                        const nlohmann::json update)
{
  std::lock_guard<std::recursive_mutex>(this->ov_tracks_mutex_);
  this->ovTracks_[id].merge_patch(update);
}

std::optional<const DigitalStage::ov_track_t>
DigitalStage::Store::readOvTrack(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->ov_tracks_mutex_);
  if(this->ovTracks_.count(id) > 0)
    return this->ovTracks_[id].get<DigitalStage::ov_track_t>();
  return std::nullopt;
}

void DigitalStage::Store::removeOvTrack(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->ov_tracks_mutex_);
  this->ovTracks_.erase(id);
}

void DigitalStage::Store::removeOvTracks()
{
  std::lock_guard<std::recursive_mutex>(this->ov_tracks_mutex_);
  this->ovTracks_.clear();
}

void DigitalStage::Store::createRemoteOvTrack(
    const nlohmann::json remoteOvTrack)
{
  std::lock_guard<std::recursive_mutex>(this->remote_ov_tracks_mutex_);
  const std::string _id = remoteOvTrack.at("_id").get<std::string>();
  this->remoteOvTracks_[_id] = remoteOvTrack;
  this->remoteOvTrackIdsByStageMemberId_[remoteOvTrack["stageMemberId"]]
      .push_back(_id);
}

void DigitalStage::Store::updateRemoteOvTrack(const std::string& id,
                                              const nlohmann::json update)
{
  std::lock_guard<std::recursive_mutex>(this->remote_ov_tracks_mutex_);
  this->remoteOvTracks_[id].merge_patch(update);
}

std::optional<const DigitalStage::remote_ov_track_t>
DigitalStage::Store::readRemoteOvTrack(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->remote_ov_tracks_mutex_);
  if(this->remoteOvTracks_.count(id) > 0)
    return this->remoteOvTracks_[id].get<DigitalStage::remote_ov_track_t>();
  return std::nullopt;
}

void DigitalStage::Store::removeRemoteOvTrack(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->remote_ov_tracks_mutex_);
  const std::string stageMemberId =
      this->remoteOvTracks_[id].at("stageMemberId").get<std::string>();
  std::remove(this->remoteOvTrackIdsByStageMemberId_[stageMemberId].begin(),
              this->remoteOvTrackIdsByStageMemberId_[stageMemberId].end(), id);
  this->remoteOvTracks_.erase(id);
}

void DigitalStage::Store::removeRemoteOvTracks()
{
  std::lock_guard<std::recursive_mutex>(this->remote_ov_tracks_mutex_);
  this->remoteOvTracks_.clear();
  this->remoteOvTrackIdsByStageMemberId_.clear();
}

void DigitalStage::Store::createCustomRemoteOvTrackPosition(
    const nlohmann::json customRemoteOvTrack)
{
  auto lock = std::unique_lock<std::recursive_mutex>(
      this->custom_remote_ov_track_positions_mutex_);
  const std::string _id = customRemoteOvTrack.at("_id").get<std::string>();
  this->customRemoteOvTrackPositions_[_id] = customRemoteOvTrack;
  this->customOvTrackPositionIdByOvTrackId_
      [customRemoteOvTrack["remoteOvTrackId"]] = _id;
}

void DigitalStage::Store::updateCustomRemoteOvTrackPosition(
    const std::string& id, const nlohmann::json update)
{
  auto lock = std::unique_lock<std::recursive_mutex>(
      this->custom_remote_ov_track_positions_mutex_);
  this->customRemoteOvTrackPositions_[id].merge_patch(update);
}

std::optional<const DigitalStage::custom_remote_ov_track_position_t>
DigitalStage::Store::readCustomRemoteOvTrackPosition(const std::string& id)
{
  auto lock = std::unique_lock<std::recursive_mutex>(
      this->custom_remote_ov_track_positions_mutex_);
  if(this->customRemoteOvTrackPositions_.count(id) > 0)
    return this->customRemoteOvTrackPositions_[id]
        .get<DigitalStage::custom_remote_ov_track_position_t>();
  return std::nullopt;
}

void DigitalStage::Store::removeCustomRemoteOvTrackPosition(
    const std::string& id)
{
  auto lock = std::unique_lock<std::recursive_mutex>(
      this->custom_remote_ov_track_positions_mutex_);
  const std::string remoteOvTrackId = this->customRemoteOvTrackPositions_[id]
                                          .at("remoteOvTrackId")
                                          .get<std::string>();
  this->customRemoteOvTrackPositions_.erase(id);
  this->customOvTrackPositionIdByOvTrackId_[remoteOvTrackId];
}

void DigitalStage::Store::removeCustomRemoteOvTrackPositions()
{
  auto lock = std::unique_lock<std::recursive_mutex>(
      this->custom_remote_ov_track_positions_mutex_);
  this->customRemoteOvTrackPositions_.clear();
  this->customOvTrackPositionIdByOvTrackId_.clear();
}

void DigitalStage::Store::createCustomRemoteOvTrackVolume(
    const nlohmann::json customRemoteOvTrack)
{
  auto lock = std::unique_lock<std::recursive_mutex>(
      this->custom_remote_ov_track_volumes_mutex_);
  const std::string _id = customRemoteOvTrack.at("_id").get<std::string>();
  this->customRemoteOvTrackVolumes_[_id] = customRemoteOvTrack;
  this->customOvTrackVolumeIdByOvTrackId_
      [customRemoteOvTrack["remoteOvTrackId"]] = _id;
}

void DigitalStage::Store::updateCustomRemoteOvTrackVolume(
    const std::string& id, const nlohmann::json update)
{
  auto lock = std::unique_lock<std::recursive_mutex>(
      this->custom_remote_ov_track_volumes_mutex_);
  this->customRemoteOvTrackVolumes_[id].merge_patch(update);
}

std::optional<const DigitalStage::custom_remote_ov_track_volume_t>
DigitalStage::Store::readCustomRemoteOvTrackVolume(const std::string& id)
{
  auto lock = std::unique_lock<std::recursive_mutex>(
      this->custom_remote_ov_track_volumes_mutex_);
  if(this->customRemoteOvTrackVolumes_.count(id) > 0)
    return this->customRemoteOvTrackVolumes_[id]
        .get<DigitalStage::custom_remote_ov_track_volume_t>();
  return std::nullopt;
}

void DigitalStage::Store::removeCustomRemoteOvTrackVolume(const std::string& id)
{
  auto lock = std::unique_lock<std::recursive_mutex>(
      this->custom_remote_ov_track_volumes_mutex_);
  const std::string remoteOvTrackId = this->customRemoteOvTrackVolumes_[id]
                                          .at("remoteOvTrackId")
                                          .get<std::string>();
  this->customRemoteOvTrackVolumes_.erase(id);
  this->customOvTrackVolumeIdByOvTrackId_[remoteOvTrackId];
}

void DigitalStage::Store::removeCustomRemoteOvTrackVolumes()
{
  auto lock = std::unique_lock<std::recursive_mutex>(
      this->custom_remote_ov_track_volumes_mutex_);
  this->customRemoteOvTrackVolumes_.clear();
  this->customOvTrackVolumeIdByOvTrackId_.clear();
}

const std::vector<DigitalStage::ov_track_t> DigitalStage::Store::readOvTracks()
{
  std::lock_guard<std::recursive_mutex>(this->ov_tracks_mutex_);
  std::vector<DigitalStage::ov_track_t> tracks;
  for(const auto& pair : this->ovTracks_) {
    tracks.push_back(pair.second);
  }
  return tracks;
}

std::optional<const DigitalStage::stage_t>
DigitalStage::Store::getCurrentStage()
{
  std::lock_guard<std::recursive_mutex>(this->stages_mutex_);
  const std::string currentStageId = this->getCurrentStageId();
  if(!currentStageId.empty()) {
    return this->readStage(currentStageId);
  }
  return std::nullopt;
}

std::optional<const DigitalStage::stage_member_t>
DigitalStage::Store::getCurrentStageMember()
{
  if(!this->currentStageMemberId_.empty()) {
    return this->readStageMember(this->currentStageMemberId_);
  }
  return std::nullopt;
}

std::optional<const DigitalStage::custom_stage_member_position_t>
DigitalStage::Store::getCustomStageMemberPositionByStageMemberId(
    const std::string& stageMemberId)
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_positions_mutex_);
  if(this->customStageMemberPositionIdByStageMemberId_.count(stageMemberId) >
     0) {
    const std::string customStageMemberId =
        this->customStageMemberPositionIdByStageMemberId_[stageMemberId];
    std::optional<const DigitalStage::custom_stage_member_position_t>
        customStageMember =
            this->readCustomStageMemberPosition(customStageMemberId);
    return customStageMember;
  }
  return std::nullopt;
}
std::optional<const DigitalStage::custom_stage_member_volume_t>
DigitalStage::Store::getCustomStageMemberVolumeByStageMemberId(
    const std::string& stageMemberId)
{
  std::lock_guard<std::recursive_mutex>(
      this->custom_stage_member_volumes_mutex_);
  if(this->customStageMemberVolumeIdByStageMemberId_.count(stageMemberId) > 0) {
    const std::string customStageMemberId =
        this->customStageMemberVolumeIdByStageMemberId_[stageMemberId];
    std::optional<const DigitalStage::custom_stage_member_volume_t>
        customStageMember =
            this->readCustomStageMemberVolume(customStageMemberId);
    return customStageMember;
  }
  return std::nullopt;
}

std::optional<const DigitalStage::custom_group_position_t>
DigitalStage::Store::getCustomGroupPositionByGroupId(
    const std::string& customStageMemberId)
{
  std::lock_guard<std::recursive_mutex>(this->custom_group_positions_mutex_);
  if(this->customGroupPositionIdByGroupId_.count(customStageMemberId) > 0) {
    return this->readCustomGroupPosition(
        this->customGroupPositionIdByGroupId_[customStageMemberId]);
  }
  return std::nullopt;
}
std::optional<const DigitalStage::custom_group_volume_t>
DigitalStage::Store::getCustomGroupVolumeByGroupId(
    const std::string& customStageMemberId)
{
  std::lock_guard<std::recursive_mutex>(this->custom_group_volumes_mutex_);
  if(this->customGroupPositionIdByGroupId_.count(customStageMemberId) > 0) {
    return this->readCustomGroupVolume(
        this->customGroupVolumeIdByGroupId_[customStageMemberId]);
  }
  return std::nullopt;
}

std::optional<const DigitalStage::custom_remote_ov_track_position_t>
DigitalStage::Store::getCustomOvTrackPositionByOvTrackId(
    const std::string& ovTrackId)
{
  auto lock = std::unique_lock<std::recursive_mutex>(
      this->custom_remote_ov_track_positions_mutex_);
  if(this->customOvTrackPositionIdByOvTrackId_.count(ovTrackId) > 0) {
    return this->readCustomRemoteOvTrackPosition(
        this->customOvTrackPositionIdByOvTrackId_[ovTrackId]);
  }
  return std::nullopt;
}

std::optional<const DigitalStage::custom_remote_ov_track_volume_t>
DigitalStage::Store::getCustomOvTrackVolumeByOvTrackId(
    const std::string& ovTrackId)
{
  auto lock = std::unique_lock<std::recursive_mutex>(
      this->custom_remote_ov_track_volumes_mutex_);
  if(this->customOvTrackVolumeIdByOvTrackId_.count(ovTrackId) > 0) {
    return this->readCustomRemoteOvTrackVolume(
        this->customOvTrackVolumeIdByOvTrackId_[ovTrackId]);
  }
  return std::nullopt;
}

const std::vector<DigitalStage::remote_ov_track_t>
DigitalStage::Store::getRemoteOvTracksByStageMemberId(
    const std::string& stageMemberId)
{
  std::lock_guard<std::recursive_mutex>(this->remote_ov_tracks_mutex_);
  std::vector<DigitalStage::remote_ov_track_t> remoteTracks;
  if(this->remoteOvTrackIdsByStageMemberId_.count(stageMemberId) > 0) {
    for(const std::string& remoteOvTrackId :
        this->remoteOvTrackIdsByStageMemberId_[stageMemberId]) {
      const auto remoteOvTrack = this->readRemoteOvTrack(remoteOvTrackId);
      if(remoteOvTrack) {
        remoteTracks.push_back(*remoteOvTrack);
      }
    }
  }
  return remoteTracks;
}

void DigitalStage::Store::createUser(const nlohmann::json user)
{
  std::lock_guard<std::recursive_mutex>(this->users_mutex_);
  const std::string _id = user.at("_id").get<std::string>();
  this->users_[_id] = user;
}

void DigitalStage::Store::updateUser(const std::string& id,
                                     const nlohmann::json update)
{
  std::lock_guard<std::recursive_mutex>(this->users_mutex_);
  this->users_[id].merge_patch(update);
}

std::optional<const DigitalStage::user_t>
DigitalStage::Store::readUser(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->users_mutex_);
  if(this->users_.count(id) > 0)
    return this->users_[id].get<DigitalStage::user_t>();
  return std::nullopt;
}

void DigitalStage::Store::removeUser(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->users_mutex_);
  this->users_.erase(id);
}

void DigitalStage::Store::removeUsers()
{
  std::lock_guard<std::recursive_mutex>(this->users_mutex_);
  this->users_.clear();
}

void DigitalStage::Store::dump()
{
  std::cout << "----- STATISTICS: ------" << std::endl;

  auto lock1 = std::unique_lock<std::recursive_mutex>(this->users_mutex_);
  auto lock2 = std::unique_lock<std::recursive_mutex>(this->stages_mutex_);
  auto lock3 = std::unique_lock<std::recursive_mutex>(this->groups_mutex_);
  auto lock4 =
      std::unique_lock<std::recursive_mutex>(this->stage_members_mutex_);
  std::cout << this->users_.size() << " users" << std::endl;
  std::cout << this->stages_.size() << " stages" << std::endl;
  std::cout << this->groups_.size() << " groups" << std::endl;
  std::cout << this->stageMembers_.size() << " stage members" << std::endl;
}