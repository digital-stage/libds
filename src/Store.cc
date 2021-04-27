
#include "DigitalStage/Api/Store.h"

using namespace DigitalStage::Api;
using namespace DigitalStage::Types;

std::optional<const device_t>
Store::getLocalDevice() const
{
  std::lock_guard<std::recursive_mutex>(this->local_device_id_mutex_);
  if(localDeviceId_) {
    return this->getDevice(*localDeviceId_);
  }
  return std::nullopt;
}
std::optional<std::string> Store::getLocalDeviceId() const
{
  std::lock_guard<std::recursive_mutex>(this->local_device_id_mutex_);
  return localDeviceId_;
}

void Store::setLocalDeviceId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->local_device_id_mutex_);
  localDeviceId_ = id;
}

std::optional<std::string> Store::getStageMemberId() const
{
  std::lock_guard<std::recursive_mutex>(this->stageMemberId_mutex_);
  return this->stageMemberId_;
}

void Store::setStageMemberId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->stageMemberId_mutex_);
  stageMemberId_ = id;
}

std::optional<std::string> Store::getUserId() const
{
  std::lock_guard<std::recursive_mutex>(this->userId_mutex_);
  return userId_;
}

void Store::setUserId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->userId_mutex_);
  userId_ = id;
}

std::optional<std::string> Store::getStageId() const
{
  std::lock_guard<std::recursive_mutex>(this->stageId_mutex_);
  return stageId_;
}

void Store::setStageId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->stageId_mutex_);
  stageId_ = id;
}

void Store::resetStageId()
{
  std::lock_guard<std::recursive_mutex>(this->stageId_mutex_);
  stageId_ = std::nullopt;
}

std::optional<std::string> Store::getGroupId() const
{
  std::lock_guard<std::recursive_mutex>(this->groupId_mutex_);
  return groupId_;
}

void Store::setGroupId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->groupId_mutex_);
  groupId_ = id;
}

void Store::resetGroupId()
{
  std::lock_guard<std::recursive_mutex>(this->groupId_mutex_);
  groupId_ = std::nullopt;
}

void Store::createGroup(const nlohmann::json payload)
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

void Store::removeGroup(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  const std::string stageId =
      this->groups_.at(id)["stageId"].get<std::string>();
  this->groups_.erase(id);
  groupIds_by_stages_[stageId].erase(id);
}

const std::vector<group_t>
Store::getGroupsByStage(const std::string& stageId) const
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  auto groups = std::vector<group_t>();
  if(this->groupIds_by_stages_.count(stageId) > 0) {
    auto groupIds = this->groupIds_by_stages_.at(stageId);
    for(const auto& groupId : groupIds) {
      auto group = getGroup(groupId);
      if(group) {
        groups.push_back(*group);
      }
    }
  }
  return groups;
}

void Store::createStageMember(const nlohmann::json payload)
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

void Store::removeStageMember(const std::string& id)
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

const std::vector<stage_member_t>
Store::getStageMembersByStage(const std::string& stageId) const
{
  std::lock_guard<std::recursive_mutex>(this->stageMembers_mutex_);
  auto vector = std::vector<stage_member_t>();
  if(this->stageMemberIds_by_stages_.count(stageId) > 0) {
    auto stageMemberIds = this->stageMemberIds_by_stages_.at(stageId);
    for(const auto& stageMemberId : stageMemberIds) {
      auto stageMember = getStageMember(stageMemberId);
      if(stageMember) {
        vector.push_back(*stageMember);
      }
    }
  }
  return vector;
}

const std::vector<stage_member_t>
Store::getStageMembersByGroup(const std::string& groupId) const
{
  std::lock_guard<std::recursive_mutex>(this->stageMembers_mutex_);
  auto vector = std::vector<stage_member_t>();
  if(this->stageMemberIds_by_groups_.count(groupId) > 0) {
    auto stageMemberIds = this->stageMemberIds_by_groups_.at(groupId);
    for(const auto& stageMemberId : stageMemberIds) {
      auto stageMember = getStageMember(stageMemberId);
      if(stageMember) {
        vector.push_back(*stageMember);
      }
    }
  }
  return vector;
}

std::optional<const custom_group_position_t>
Store::getCustomGroupPositionByGroupAndDevice(
    const std::string& groupId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex>(this->customGroupPositions_mutex_);
  if(this->customGroupPositionIds_by_Group_and_Device_.count(groupId) > 0) {
    if(this->customGroupPositionIds_by_Group_and_Device_.at(groupId).count(
           deviceId) > 0) {
      return getCustomGroupPosition(
          this->customGroupPositionIds_by_Group_and_Device_.at(groupId).at(
              deviceId));
    }
  }
  return std::nullopt;
}

void Store::createCustomGroupPosition(const json payload)
{
  std::lock_guard<std::recursive_mutex>(this->customGroupPositions_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  customGroupPositions_[_id] = payload;
  const std::string groupId = payload.at("groupId").get<std::string>();
  const std::string deviceId = payload.at("deviceId").get<std::string>();
  if(customGroupPositionIds_by_Group_and_Device_.count(groupId) == 0) {
    customGroupPositionIds_by_Group_and_Device_[groupId] =
        std::map<std::string, std::string>();
  }
  customGroupPositionIds_by_Group_and_Device_[groupId][deviceId] = _id;
}

void Store::removeCustomGroupPosition(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->customGroupPositions_mutex_);
  const std::string groupId =
      this->customGroupPositions_.at(id)["groupId"].get<std::string>();
  const std::string deviceId =
      this->customGroupPositions_.at(id)["deviceId"].get<std::string>();
  customGroupPositionIds_by_Group_and_Device_[groupId].erase(deviceId);
  customGroupPositions_.erase(id);
}

std::optional<const custom_group_volume_t>
Store::getCustomGroupVolumeByGroupAndDevice(
    const std::string& groupId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex>(this->customGroupVolumes_mutex_);
  if(this->customGroupVolumeIds_by_Group_and_Device_.count(groupId) > 0) {
    if(this->customGroupVolumeIds_by_Group_and_Device_.at(groupId).count(
           deviceId) > 0) {
      return getCustomGroupVolume(
          this->customGroupVolumeIds_by_Group_and_Device_.at(groupId).at(
              deviceId));
    }
  }
  return std::nullopt;
}

void Store::createCustomGroupVolume(const json payload)
{
  std::lock_guard<std::recursive_mutex>(this->customGroupVolumes_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  customGroupVolumes_[_id] = payload;
  const std::string groupId = payload.at("groupId").get<std::string>();
  const std::string deviceId = payload.at("deviceId").get<std::string>();
  if(customGroupVolumeIds_by_Group_and_Device_.count(groupId) == 0) {
    customGroupVolumeIds_by_Group_and_Device_[groupId] =
        std::map<std::string, std::string>();
  }
  customGroupVolumeIds_by_Group_and_Device_[groupId][deviceId] = _id;
}

void Store::removeCustomGroupVolume(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->customGroupVolumes_mutex_);
  const std::string groupId =
      this->customGroupVolumes_.at(id)["groupId"].get<std::string>();
  const std::string deviceId =
      this->customStageMemberVolumes_.at(id)["deviceId"].get<std::string>();
  customGroupVolumeIds_by_Group_and_Device_[groupId].erase(deviceId);
  customGroupVolumes_.erase(id);
}

std::optional<const custom_stage_member_position_t>
Store::getCustomStageMemberPositionByStageMemberAndDevice(
    const std::string& stageMemberId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex>(
      this->customStageMemberPositions_mutex_);
  if(this->customStageMemberPositionIds_by_StageMember_and_Device_.count(
         stageMemberId) > 0) {
    if(this->customStageMemberPositionIds_by_StageMember_and_Device_
           .at(stageMemberId)
           .count(deviceId) > 0) {
      return getCustomStageMemberPosition(
          this->customStageMemberPositionIds_by_StageMember_and_Device_
              .at(stageMemberId)
              .at(deviceId));
    }
  }
  return std::nullopt;
}

void Store::createCustomStageMemberPosition(const json payload)
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
        std::map<std::string, std::string>();
  }
  customStageMemberPositionIds_by_StageMember_and_Device_[stageMemberId]
                                                         [deviceId] = _id;
}

void Store::removeCustomStageMemberPosition(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(
      this->customStageMemberPositions_mutex_);
  const std::string stageMemberId =
      this->customStageMemberPositions_.at(id)["stageMemberId"]
          .get<std::string>();
  const std::string deviceId =
      this->customStageMemberPositions_.at(id)["deviceId"].get<std::string>();
  customStageMemberPositionIds_by_StageMember_and_Device_[stageMemberId].erase(
      deviceId);
  customStageMemberPositions_.erase(id);
}

std::optional<const custom_stage_member_volume_t>
Store::getCustomStageMemberVolumeByStageMemberAndDevice(
    const std::string& stageMemberId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex>(this->customStageMemberVolumes_mutex_);
  if(this->customStageMemberVolumeIds_by_StageMember_and_Device_.count(
         stageMemberId) > 0) {
    if(this->customStageMemberVolumeIds_by_StageMember_and_Device_
           .at(stageMemberId)
           .count(deviceId) > 0) {
      return getCustomStageMemberVolume(
          this->customStageMemberVolumeIds_by_StageMember_and_Device_
              .at(stageMemberId)
              .at(deviceId));
    }
  }
  return std::nullopt;
}

void Store::createCustomStageMemberVolume(const json payload)
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
        std::map<std::string, std::string>();
  }
  customStageMemberVolumeIds_by_StageMember_and_Device_[stageMemberId]
                                                       [deviceId] = _id;
}

void Store::removeCustomStageMemberVolume(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->customStageMemberVolumes_mutex_);
  const std::string stageMemberId =
      this->customStageMemberVolumes_.at(id)["stageMemberId"]
          .get<std::string>();
  const std::string deviceId =
      this->customStageMemberVolumes_.at(id)["deviceId"].get<std::string>();
  customStageMemberVolumeIds_by_StageMember_and_Device_[stageMemberId].erase(
      deviceId);
  customStageMemberVolumes_.erase(id);
}

std::optional<const custom_remote_audio_track_position_t>
Store::
    getCustomRemoteAudioTrackPositionByRemoteAudioTrackAndDevice(
        const std::string& remoteAudioTrackId,
        const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex>(
      this->customRemoteAudioTrackPositions_mutex_);
  if(this->customRemoteAudioTrackPositionIds_by_RemoteAudioTrack_and_Device_
         .count(remoteAudioTrackId) > 0) {
    if(this->customRemoteAudioTrackPositionIds_by_RemoteAudioTrack_and_Device_
           .at(remoteAudioTrackId)
           .count(deviceId) > 0) {
      return getCustomRemoteAudioTrackPosition(
          this->customRemoteAudioTrackPositionIds_by_RemoteAudioTrack_and_Device_
              .at(remoteAudioTrackId)
              .at(deviceId));
    }
  }
  return std::nullopt;
}

void Store::createCustomRemoteAudioTrackPosition(
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
        [remoteAudioTrackId] = std::map<std::string, std::string>();
  }
  customRemoteAudioTrackPositionIds_by_RemoteAudioTrack_and_Device_
      [remoteAudioTrackId][deviceId] = _id;
}

void Store::removeCustomRemoteAudioTrackPosition(
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
      [remoteAudioTrackId]
          .erase(deviceId);
  customRemoteAudioTrackPositions_.erase(id);
}

std::optional<const custom_remote_audio_track_volume_t>
Store::getCustomRemoteAudioTrackVolumeByRemoteAudioTrackAndDevice(
    const std::string& remoteAudioTrackId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex>(
      this->customRemoteAudioTrackVolumes_mutex_);
  if(this->customRemoteAudioTrackVolumeIds_by_RemoteAudioTrack_and_Device_
         .count(remoteAudioTrackId) > 0) {
    if(this->customRemoteAudioTrackVolumeIds_by_RemoteAudioTrack_and_Device_
           .at(remoteAudioTrackId)
           .count(deviceId) > 0) {
      return getCustomRemoteAudioTrackVolume(
          this->customRemoteAudioTrackVolumeIds_by_RemoteAudioTrack_and_Device_
              .at(remoteAudioTrackId)
              .at(deviceId));
    }
  }
  return std::nullopt;
}

void Store::createCustomRemoteAudioTrackVolume(const json payload)
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
        [remoteAudioTrackId] = std::map<std::string, std::string>();
  }
  customRemoteAudioTrackVolumeIds_by_RemoteAudioTrack_and_Device_
      [remoteAudioTrackId][deviceId] = _id;
}

void Store::removeCustomRemoteAudioTrackVolume(
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
      [remoteAudioTrackId]
          .erase(deviceId);
  customRemoteAudioTrackVolumes_.erase(id);
}

const std::vector<remote_video_track_t>
Store::getRemoteVideoTracksByStageMember(
    const std::string& stageMemberId) const
{
  std::lock_guard<std::recursive_mutex>(this->remoteVideoTracks_mutex_);
  auto items = std::vector<remote_video_track_t>();
  if(this->remoteVideoTrackIds_by_StageMember_.count(stageMemberId) > 0) {
    auto ids = this->remoteVideoTrackIds_by_StageMember_.at(stageMemberId);
    for(const auto& id : ids) {
      auto group = getRemoteVideoTrack(id);
      if(group) {
        items.push_back(*group);
      }
    }
  }
  return items;
}
void Store::createRemoteVideoTrack(const json payload)
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
void Store::removeRemoteVideoTrack(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->remoteVideoTracks_mutex_);
  const std::string stageMemberId =
      this->remoteVideoTracks_.at(id)["stageMemberId"].get<std::string>();
  this->remoteVideoTracks_.erase(id);
  remoteVideoTrackIds_by_StageMember_[stageMemberId].erase(id);
}

const std::vector<remote_audio_track_t>
Store::getRemoteAudioTracksByStageMember(
    const std::string& stageMemberId) const
{
  std::lock_guard<std::recursive_mutex>(this->remoteAudioTracks_mutex_);
  auto items = std::vector<remote_audio_track_t>();
  if(this->remoteAudioTrackIds_by_StageMember_.count(stageMemberId) > 0) {
    auto ids = this->remoteAudioTrackIds_by_StageMember_.at(stageMemberId);
    for(const auto& id : ids) {
      auto group = getRemoteAudioTrack(id);
      if(group) {
        items.push_back(*group);
      }
    }
  }
  return items;
}
void Store::createRemoteAudioTrack(const json payload)
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
void Store::removeRemoteAudioTrack(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->remoteAudioTracks_mutex_);
  const std::string stageMemberId =
      this->remoteAudioTracks_.at(id)["stageMemberId"].get<std::string>();
  this->remoteAudioTracks_.erase(id);
  remoteAudioTrackIds_by_StageMember_[stageMemberId].erase(id);
}