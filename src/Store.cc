
#include "Store.h"

std::optional<const DigitalStage::device_t>
DigitalStage::Store::getLocalDevice() const
{
  std::lock_guard<std::recursive_mutex>(this->local_device_id_mutex_);
  if(localDeviceId_) {
    return this->getDevice(localDeviceId_.value());
  }
  return std::nullopt;
}
std::optional<std::string> DigitalStage::Store::getLocalDeviceId() const
{
  std::lock_guard<std::recursive_mutex>(this->local_device_id_mutex_);
  return localDeviceId_;
}

void DigitalStage::Store::setLocalDeviceId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->local_device_id_mutex_);
  localDeviceId_ = id;
}

std::optional<std::string> DigitalStage::Store::getUserId() const
{
  std::lock_guard<std::recursive_mutex>(this->userId_mutex_);
  return userId_;
}

void DigitalStage::Store::setUserId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->userId_mutex_);
  userId_ = id;
}

std::optional<std::string> DigitalStage::Store::getStageId() const
{
  std::lock_guard<std::recursive_mutex>(this->stageId_mutex_);
  return stageId_;
}

void DigitalStage::Store::setStageId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->stageId_mutex_);
  stageId_ = id;
}

void DigitalStage::Store::resetStageId()
{
  std::lock_guard<std::recursive_mutex>(this->stageId_mutex_);
  stageId_ = std::nullopt;
}

std::optional<std::string> DigitalStage::Store::getGroupId() const
{
  std::lock_guard<std::recursive_mutex>(this->groupId_mutex_);
  return groupId_;
}

void DigitalStage::Store::setGroupId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->groupId_mutex_);
  groupId_ = id;
}

void DigitalStage::Store::resetGroupId()
{
  std::lock_guard<std::recursive_mutex>(this->groupId_mutex_);
  groupId_ = std::nullopt;
}

void DigitalStage::Store::createGroup(const nlohmann::json payload)
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  groups_[_id] = payload;
  const std::string stageId = payload.at("stageId").get<std::string>();
  if(groupIds_by_stages_.count(stageId) == 0) {
    groupIds_by_stages_[stageId] = std::set<std::string>();
  }
  groupIds_by_stages_[stageId].insert(_id);
}

void DigitalStage::Store::removeGroup(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  const std::string stageId =
      this->groups_.at(id)["stageId"].get<std::string>();
  this->groups_.erase(id);
  groupIds_by_stages_[stageId].erase(id);
}

std::vector<const DigitalStage::group_t>
DigitalStage::Store::getGroupsByStage(const std::string& stageId) const
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  auto groups = std::vector<const DigitalStage::group_t>();
  if(this->groupIds_by_stages_.count(stageId) > 0) {
    auto groupIds = this->groupIds_by_stages_.at(stageId);
    for(const auto& groupId : groupIds) {
      auto group = getGroup(groupId);
      if(group) {
        groups.push_back(group.value());
      }
    }
  }
  return groups;
}

void DigitalStage::Store::createStageMember(const nlohmann::json payload)
{
  std::lock_guard<std::recursive_mutex>(this->stageMembers_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  stageMembers_[_id] = payload;
  const std::string stageId = payload.at("stageId").get<std::string>();
  const std::string groupId = payload.at("groupId").get<std::string>();
  if(stageMemberIds_by_stages_.count(stageId) == 0) {
    stageMemberIds_by_stages_[stageId] = std::set<std::string>();
  }
  stageMemberIds_by_stages_[stageId].insert(_id);
  if(stageMemberIds_by_groups_.count(groupId) == 0) {
    stageMemberIds_by_groups_[stageId] = std::set<std::string>();
  }
  stageMemberIds_by_groups_[groupId].insert(_id);
}

void DigitalStage::Store::removeStageMember(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->stageMembers_mutex_);
  const std::string stageId =
      this->stageMembers_.at(id)["stageId"].get<std::string>();
  const std::string groupId =
      this->stageMembers_.at(id)["groupId"].get<std::string>();
  this->stageMembers_.erase(id);
  stageMemberIds_by_stages_[stageId].erase(id);
  stageMemberIds_by_groups_[groupId].erase(id);
}

std::vector<const DigitalStage::stage_member_t>
DigitalStage::Store::getStageMembersByStage(const std::string& stageId) const
{
  std::lock_guard<std::recursive_mutex>(this->stageMembers_mutex_);
  auto vector = std::vector<const DigitalStage::stage_member_t>();
  if(this->stageMemberIds_by_stages_.count(stageId) > 0) {
    auto stageMemberIds = this->stageMemberIds_by_stages_.at(stageId);
    for(const auto& stageMemberId : stageMemberIds) {
      auto stageMember = getStageMember(stageMemberId);
      if(stageMember) {
        vector.push_back(stageMember.value());
      }
    }
  }
  return vector;
}

std::vector<const DigitalStage::stage_member_t>
DigitalStage::Store::getStageMembersByGroup(const std::string& groupId) const
{
  std::lock_guard<std::recursive_mutex>(this->stageMembers_mutex_);
  auto vector = std::vector<const DigitalStage::stage_member_t>();
  if(this->stageMemberIds_by_groups_.count(groupId) > 0) {
    auto stageMemberIds = this->stageMemberIds_by_groups_.at(groupId);
    for(const auto& stageMemberId : stageMemberIds) {
      auto stageMember = getStageMember(stageMemberId);
      if(stageMember) {
        vector.push_back(stageMember.value());
      }
    }
  }
  return vector;
}

std::vector<const DigitalStage::custom_group_position_t>
DigitalStage::Store::getCustomGroupPositionByGroupAndDevice(
    const std::string& groupId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex>(this->customGroupPositions_mutex_);
  auto vector = std::vector<const DigitalStage::custom_group_position_t>();
  if(this->customGroupPositionIds_by_Group_and_Device_.count(groupId) > 0) {
    if(this->customGroupPositionIds_by_Group_and_Device_.at(groupId).count(
           deviceId) > 0) {
      for(const auto& id :
          this->customGroupPositionIds_by_Group_and_Device_.at(groupId).at(
              deviceId)) {
        auto item = getCustomGroupPosition(id);
        vector.push_back(item.value());
      }
    }
  }
  return vector;
}

void DigitalStage::Store::createCustomGroupPosition(const json payload)
{
  std::lock_guard<std::recursive_mutex>(this->customGroupPositions_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  customGroupPositions_[_id] = payload;
  const std::string groupId = payload.at("groupId").get<std::string>();
  const std::string deviceId = payload.at("deviceId").get<std::string>();
  if(customGroupPositionIds_by_Group_and_Device_.count(groupId) == 0) {
    customGroupPositionIds_by_Group_and_Device_[groupId] =
        std::map<std::string, std::set<std::string>>();
  }
  if(customGroupPositionIds_by_Group_and_Device_[groupId].count(deviceId) ==
     0) {
    customGroupPositionIds_by_Group_and_Device_[groupId][deviceId] =
        std::set<std::string>();
  }
  customGroupPositionIds_by_Group_and_Device_[groupId][deviceId].insert(_id);
}

void DigitalStage::Store::removeCustomGroupPosition(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->customGroupPositions_mutex_);
  const std::string groupId =
      this->customGroupPositions_.at(id)["groupId"].get<std::string>();
  const std::string deviceId =
      this->customGroupPositions_.at(id)["groupId"].get<std::string>();
  customGroupPositionIds_by_Group_and_Device_[groupId][deviceId].erase(id);
  customGroupPositions_.erase(id);
}

std::vector<const DigitalStage::custom_group_volume_t>
DigitalStage::Store::getCustomGroupVolumeByGroupAndDevice(
    const std::string& groupId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex>(this->customGroupVolumes_mutex_);
  auto vector = std::vector<const DigitalStage::custom_group_volume_t>();
  if(this->customGroupVolumeIds_by_Group_and_Device_.count(groupId) > 0) {
    if(this->customGroupVolumeIds_by_Group_and_Device_.at(groupId).count(
           deviceId) > 0) {
      for(const auto& id :
          this->customGroupVolumeIds_by_Group_and_Device_.at(groupId).at(
              deviceId)) {
        auto item = getCustomGroupVolume(id);
        vector.push_back(item.value());
      }
    }
  }
  return vector;
}

void DigitalStage::Store::createCustomGroupVolume(const json payload)
{
  std::lock_guard<std::recursive_mutex>(this->customGroupVolumes_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  customGroupVolumes_[_id] = payload;
  const std::string groupId = payload.at("groupId").get<std::string>();
  const std::string deviceId = payload.at("deviceId").get<std::string>();
  if(customGroupVolumeIds_by_Group_and_Device_.count(groupId) == 0) {
    customGroupVolumeIds_by_Group_and_Device_[groupId] =
        std::map<std::string, std::set<std::string>>();
  }
  if(customGroupVolumeIds_by_Group_and_Device_[groupId].count(deviceId) == 0) {
    customGroupVolumeIds_by_Group_and_Device_[groupId][deviceId] =
        std::set<std::string>();
  }
  customGroupVolumeIds_by_Group_and_Device_[groupId][deviceId].insert(_id);
}

void DigitalStage::Store::removeCustomGroupVolume(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->customGroupVolumes_mutex_);
  const std::string groupId =
      this->customGroupVolumes_.at(id)["groupId"].get<std::string>();
  const std::string deviceId =
      this->customStageMemberVolumes_.at(id)["deviceId"].get<std::string>();
  customGroupVolumeIds_by_Group_and_Device_[groupId][deviceId].erase(id);
  customGroupVolumes_.erase(id);
}

std::vector<const DigitalStage::custom_stage_member_position_t>
DigitalStage::Store::getCustomStageMemberPositionByStageMemberAndDevice(
    const std::string& stageMemberId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex>(
      this->customStageMemberPositions_mutex_);
  auto vector =
      std::vector<const DigitalStage::custom_stage_member_position_t>();
  if(this->customStageMemberPositionIds_by_StageMember_and_Device_.count(
         stageMemberId) > 0) {
    if(this->customStageMemberPositionIds_by_StageMember_and_Device_
           .at(stageMemberId)
           .count(deviceId) > 0) {
      for(const auto& id :
          this->customStageMemberPositionIds_by_StageMember_and_Device_
              .at(stageMemberId)
              .at(deviceId)) {
        auto item = getCustomStageMemberPosition(id);
        vector.push_back(item.value());
      }
    }
  }
  return vector;
}

void DigitalStage::Store::createCustomStageMemberPosition(const json payload)
{
  std::lock_guard<std::recursive_mutex>(
      this->customStageMemberPositions_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  customStageMemberPositions_[_id] = payload;
  const std::string stageMemberId =
      payload.at("stageMemberId").get<std::string>();
  const std::string deviceId = payload.at("deviceId").get<std::string>();
  if(customStageMemberPositionIds_by_StageMember_and_Device_.count(
         stageMemberId) == 0) {
    customStageMemberPositionIds_by_StageMember_and_Device_[stageMemberId] =
        std::map<std::string, std::set<std::string>>();
  }
  if(customStageMemberPositionIds_by_StageMember_and_Device_[stageMemberId]
         .count(deviceId) == 0) {
    customStageMemberPositionIds_by_StageMember_and_Device_
        [stageMemberId][deviceId] = std::set<std::string>();
  }
  customStageMemberPositionIds_by_StageMember_and_Device_[stageMemberId]
                                                         [deviceId]
                                                             .insert(_id);
}

void DigitalStage::Store::removeCustomStageMemberPosition(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(
      this->customStageMemberPositions_mutex_);
  const std::string stageMemberId =
      this->customStageMemberPositions_.at(id)["stageMemberId"]
          .get<std::string>();
  const std::string deviceId =
      this->customStageMemberPositions_.at(id)["deviceId"].get<std::string>();
  customStageMemberPositionIds_by_StageMember_and_Device_[stageMemberId]
                                                         [deviceId]
                                                             .erase(id);
  customStageMemberPositions_.erase(id);
}

std::vector<const DigitalStage::custom_stage_member_volume_t>
DigitalStage::Store::getCustomStageMemberVolumeByStageMemberAndDevice(
    const std::string& stageMemberId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex>(this->customStageMemberVolumes_mutex_);
  auto vector = std::vector<const DigitalStage::custom_stage_member_volume_t>();
  if(this->customStageMemberVolumeIds_by_StageMember_and_Device_.count(
         stageMemberId) > 0) {
    if(this->customStageMemberVolumeIds_by_StageMember_and_Device_
           .at(stageMemberId)
           .count(deviceId) > 0) {
      for(const auto& id :
          this->customStageMemberVolumeIds_by_StageMember_and_Device_
              .at(stageMemberId)
              .at(deviceId)) {
        auto item = getCustomStageMemberVolume(id);
        vector.push_back(item.value());
      }
    }
  }
  return vector;
}

void DigitalStage::Store::createCustomStageMemberVolume(const json payload)
{
  std::lock_guard<std::recursive_mutex>(this->customStageMemberVolumes_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  customStageMemberVolumes_[_id] = payload;
  const std::string stageMemberId =
      payload.at("stageMemberId").get<std::string>();
  const std::string deviceId = payload.at("deviceId").get<std::string>();
  if(customStageMemberVolumeIds_by_StageMember_and_Device_.count(
         stageMemberId) == 0) {
    customStageMemberVolumeIds_by_StageMember_and_Device_[stageMemberId] =
        std::map<std::string, std::set<std::string>>();
  }
  if(customStageMemberVolumeIds_by_StageMember_and_Device_[stageMemberId].count(
         deviceId) == 0) {
    customStageMemberVolumeIds_by_StageMember_and_Device_
        [stageMemberId][deviceId] = std::set<std::string>();
  }
  customStageMemberVolumeIds_by_StageMember_and_Device_[stageMemberId][deviceId]
      .insert(_id);
}

void DigitalStage::Store::removeCustomStageMemberVolume(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->customStageMemberVolumes_mutex_);
  const std::string stageMemberId =
      this->customStageMemberVolumes_.at(id)["stageMemberId"]
          .get<std::string>();
  const std::string deviceId =
      this->customStageMemberVolumes_.at(id)["deviceId"].get<std::string>();
  customStageMemberVolumeIds_by_StageMember_and_Device_[stageMemberId][deviceId]
      .erase(id);
  customStageMemberVolumes_.erase(id);
}

std::vector<const DigitalStage::custom_remote_audio_track_position_t>
DigitalStage::Store::
    getCustomRemoteAudioTrackPositionByRemoteAudioTrackAndDevice(
        const std::string& remoteAudioTrackId,
        const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex>(
      this->customRemoteAudioTrackPositions_mutex_);
  auto vector =
      std::vector<const DigitalStage::custom_remote_audio_track_position_t>();
  if(this->customRemoteAudioTrackPositionIds_by_RemoteAudioTrack_and_Device_
         .count(remoteAudioTrackId) > 0) {
    if(this->customRemoteAudioTrackPositionIds_by_RemoteAudioTrack_and_Device_
           .at(remoteAudioTrackId)
           .count(deviceId) > 0) {
      for(const auto& id :
          this->customRemoteAudioTrackPositionIds_by_RemoteAudioTrack_and_Device_
              .at(remoteAudioTrackId)
              .at(deviceId)) {
        auto item = getCustomRemoteAudioTrackPosition(id);
        vector.push_back(item.value());
      }
    }
  }
  return vector;
}

void DigitalStage::Store::createCustomRemoteAudioTrackPosition(
    const json payload)
{
  std::lock_guard<std::recursive_mutex>(
      this->customRemoteAudioTrackPositions_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  customRemoteAudioTrackPositions_[_id] = payload;
  const std::string remoteAudioTrackId =
      payload.at("remoteAudioTrackId").get<std::string>();
  const std::string deviceId = payload.at("deviceId").get<std::string>();
  if(customRemoteAudioTrackPositionIds_by_RemoteAudioTrack_and_Device_.count(
         remoteAudioTrackId) == 0) {
    customRemoteAudioTrackPositionIds_by_RemoteAudioTrack_and_Device_
        [remoteAudioTrackId] = std::map<std::string, std::set<std::string>>();
  }
  if(customRemoteAudioTrackPositionIds_by_RemoteAudioTrack_and_Device_
         [remoteAudioTrackId]
             .count(deviceId) == 0) {
    customRemoteAudioTrackPositionIds_by_RemoteAudioTrack_and_Device_
        [remoteAudioTrackId][deviceId] = std::set<std::string>();
  }
  customRemoteAudioTrackPositionIds_by_RemoteAudioTrack_and_Device_
      [remoteAudioTrackId][deviceId]
          .insert(_id);
}

void DigitalStage::Store::removeCustomRemoteAudioTrackPosition(
    const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(
      this->customRemoteAudioTrackPositions_mutex_);
  const std::string remoteAudioTrackId =
      this->customRemoteAudioTrackPositions_.at(id)["remoteAudioTrackId"]
          .get<std::string>();
  const std::string deviceId =
      this->customRemoteAudioTrackPositions_.at(id)["remoteAudioTrackId"]
          .get<std::string>();
  customRemoteAudioTrackPositionIds_by_RemoteAudioTrack_and_Device_
      [remoteAudioTrackId][deviceId]
          .erase(id);
  customRemoteAudioTrackPositions_.erase(id);
}

std::vector<const DigitalStage::custom_remote_audio_track_volume_t>
DigitalStage::Store::getCustomRemoteAudioTrackVolumeByRemoteAudioTrackAndDevice(
    const std::string& remoteAudioTrackId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex>(
      this->customRemoteAudioTrackVolumes_mutex_);
  auto vector =
      std::vector<const DigitalStage::custom_remote_audio_track_volume_t>();
  if(this->customRemoteAudioTrackVolumeIds_by_RemoteAudioTrack_and_Device_
         .count(remoteAudioTrackId) > 0) {
    if(this->customRemoteAudioTrackVolumeIds_by_RemoteAudioTrack_and_Device_
           .at(remoteAudioTrackId)
           .count(deviceId) > 0) {
      for(const auto& id :
          this->customRemoteAudioTrackVolumeIds_by_RemoteAudioTrack_and_Device_
              .at(remoteAudioTrackId)
              .at(deviceId)) {
        auto item = getCustomRemoteAudioTrackVolume(id);
        vector.push_back(item.value());
      }
    }
  }
  return vector;
}

void DigitalStage::Store::createCustomRemoteAudioTrackVolume(const json payload)
{
  std::lock_guard<std::recursive_mutex>(
      this->customRemoteAudioTrackVolumes_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  customRemoteAudioTrackVolumes_[_id] = payload;
  const std::string remoteAudioTrackId =
      payload.at("remoteAudioTrackId").get<std::string>();
  const std::string deviceId = payload.at("deviceId").get<std::string>();
  if(customRemoteAudioTrackVolumeIds_by_RemoteAudioTrack_and_Device_.count(
         remoteAudioTrackId) == 0) {
    customRemoteAudioTrackVolumeIds_by_RemoteAudioTrack_and_Device_
        [remoteAudioTrackId] = std::map<std::string, std::set<std::string>>();
  }
  if(customRemoteAudioTrackVolumeIds_by_RemoteAudioTrack_and_Device_
         [remoteAudioTrackId]
             .count(deviceId) == 0) {
    customRemoteAudioTrackVolumeIds_by_RemoteAudioTrack_and_Device_
        [remoteAudioTrackId][deviceId] = std::set<std::string>();
  }
  customRemoteAudioTrackVolumeIds_by_RemoteAudioTrack_and_Device_
      [remoteAudioTrackId][deviceId]
          .insert(_id);
}

void DigitalStage::Store::removeCustomRemoteAudioTrackVolume(
    const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(
      this->customRemoteAudioTrackVolumes_mutex_);
  const std::string remoteAudioTrackId =
      this->customRemoteAudioTrackVolumes_.at(id)["remoteAudioTrackId"]
          .get<std::string>();
  const std::string deviceId =
      this->customStageMemberVolumes_.at(id)["deviceId"].get<std::string>();
  customRemoteAudioTrackVolumeIds_by_RemoteAudioTrack_and_Device_
      [remoteAudioTrackId][deviceId]
          .erase(id);
  customRemoteAudioTrackVolumes_.erase(id);
}

std::vector<const DigitalStage::remote_video_track_t>
DigitalStage::Store::getRemoteVideoTracksByStageMember(
    const std::string& stageMemberId) const
{
  std::lock_guard<std::recursive_mutex>(this->remoteVideoTracks_mutex_);
  auto items = std::vector<const DigitalStage::remote_video_track_t>();
  if(this->remoteVideoTrackIds_by_StageMember_.count(stageMemberId) > 0) {
    auto ids = this->remoteVideoTrackIds_by_StageMember_.at(stageMemberId);
    for(const auto& id : ids) {
      auto group = getRemoteVideoTrack(id);
      if(group) {
        items.push_back(group.value());
      }
    }
  }
  return items;
}
void DigitalStage::Store::createRemoteVideoTrack(const json payload)
{
  std::lock_guard<std::recursive_mutex>(this->remoteVideoTracks_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  remoteVideoTracks_[_id] = payload;
  const std::string stageMemberId =
      payload.at("stageMemberId").get<std::string>();
  if(remoteVideoTrackIds_by_StageMember_.count(stageMemberId) == 0) {
    remoteVideoTrackIds_by_StageMember_[stageMemberId] =
        std::set<std::string>();
  }
  remoteVideoTrackIds_by_StageMember_[stageMemberId].insert(_id);
}
void DigitalStage::Store::removeRemoteVideoTrack(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->remoteVideoTracks_mutex_);
  const std::string stageMemberId =
      this->remoteVideoTracks_.at(id)["stageMemberId"].get<std::string>();
  this->remoteVideoTracks_.erase(id);
  remoteVideoTrackIds_by_StageMember_[stageMemberId].erase(id);
}

std::vector<const DigitalStage::remote_audio_track_t>
DigitalStage::Store::getRemoteAudioTracksByStageMember(
    const std::string& stageMemberId) const
{
  std::lock_guard<std::recursive_mutex>(this->remoteAudioTracks_mutex_);
  auto items = std::vector<const DigitalStage::remote_audio_track_t>();
  if(this->remoteAudioTrackIds_by_StageMember_.count(stageMemberId) > 0) {
    auto ids = this->remoteAudioTrackIds_by_StageMember_.at(stageMemberId);
    for(const auto& id : ids) {
      auto group = getRemoteAudioTrack(id);
      if(group) {
        items.push_back(group.value());
      }
    }
  }
  return items;
}
void DigitalStage::Store::createRemoteAudioTrack(const json payload)
{
  std::lock_guard<std::recursive_mutex>(this->remoteAudioTracks_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  remoteAudioTracks_[_id] = payload;
  const std::string stageMemberId =
      payload.at("stageMemberId").get<std::string>();
  if(remoteAudioTrackIds_by_StageMember_.count(stageMemberId) == 0) {
    remoteAudioTrackIds_by_StageMember_[stageMemberId] =
        std::set<std::string>();
  }
  remoteAudioTrackIds_by_StageMember_[stageMemberId].insert(_id);
}
void DigitalStage::Store::removeRemoteAudioTrack(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->remoteAudioTracks_mutex_);
  const std::string stageMemberId =
      this->remoteAudioTracks_.at(id)["stageMemberId"].get<std::string>();
  this->remoteAudioTracks_.erase(id);
  remoteAudioTrackIds_by_StageMember_[stageMemberId].erase(id);
}