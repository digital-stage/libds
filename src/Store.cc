
#include "DigitalStage/Api/Store.h"

using namespace DigitalStage::Api;
using namespace DigitalStage::Types;

Store::Store() : isReady_(false) {}

std::optional<const Device> Store::getLocalDevice() const
{
  std::lock_guard<std::recursive_mutex> lock(this->local_device_id_mutex_);
  if(localDeviceId_) {
    return this->getDevice(*localDeviceId_);
  }
  return std::nullopt;
}

std::optional<std::string> Store::getLocalDeviceId() const
{
  std::lock_guard<std::recursive_mutex> lock(this->local_device_id_mutex_);
  return localDeviceId_;
}

void Store::setLocalDeviceId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(this->local_device_id_mutex_);
  localDeviceId_ = id;
}

std::optional<std::string> Store::getStageDeviceId() const
{
  std::lock_guard<std::recursive_mutex> lock(this->stage_device_id_mutex_);
  return stageDeviceId_;
}

void Store::setStageDeviceId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(this->stage_device_id_mutex_);
  stageDeviceId_ = id;
}

void Store::resetStageDeviceId()
{
  std::lock_guard<std::recursive_mutex> lock(this->stage_device_id_mutex_);
  stageDeviceId_ = std::nullopt;
}

std::optional<StageDevice> Store::getStageDevice() const
{
  std::lock_guard<std::recursive_mutex> lock(this->stage_device_id_mutex_);
  auto stageDeviceId = getStageDeviceId();
  if(stageDeviceId) {
    return getStageDevice(*stageDeviceId);
  }
  return std::nullopt;
}

[[maybe_unused]] std::optional<std::string> Store::getStageMemberId() const
{
  std::lock_guard<std::recursive_mutex> lock(this->stageMemberId_mutex_);
  return this->stageMemberId_;
}

void Store::setStageMemberId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(this->stageMemberId_mutex_);
  stageMemberId_ = id;
}

std::optional<std::string> Store::getUserId() const
{
  std::lock_guard<std::recursive_mutex> lock(this->userId_mutex_);
  return userId_;
}

void Store::setUserId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(this->userId_mutex_);
  userId_ = id;
}

std::optional<std::string> Store::getStageId() const
{
  std::lock_guard<std::recursive_mutex> lock(this->stageId_mutex_);
  return stageId_;
}

void Store::setStageId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(this->stageId_mutex_);
  stageId_ = id;
}

void Store::resetStageId()
{
  std::lock_guard<std::recursive_mutex> lock(this->stageId_mutex_);
  stageId_ = std::nullopt;
}

std::optional<std::string> Store::getGroupId() const
{
  std::lock_guard<std::recursive_mutex> lock(this->groupId_mutex_);
  return groupId_;
}

void Store::setGroupId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(this->groupId_mutex_);
  groupId_ = id;
}

void Store::resetGroupId()
{
  std::lock_guard<std::recursive_mutex> lock(this->groupId_mutex_);
  groupId_ = std::nullopt;
}

void Store::createGroup(const nlohmann::json& payload)
{
  std::lock_guard<std::recursive_mutex> lock(this->groups_mutex_);
  const auto _id = payload.at("_id").get<std::string>();
  groups_[_id] = payload;
  const auto stageId = payload.at("stageId").get<std::string>();
  if(groupIds_by_stages_.count(stageId) == 0) {
    groupIds_by_stages_[stageId] = std::set<std::string>();
  }
  groupIds_by_stages_[stageId].insert(_id);
}

void Store::removeGroup(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(this->groups_mutex_);
  const auto stageId = this->groups_.at(id)["stageId"].get<std::string>();
  this->groups_.erase(id);
  groupIds_by_stages_[stageId].erase(id);
}

std::vector<Group> Store::getGroupsByStage(const std::string& stageId) const
{
  std::lock_guard<std::recursive_mutex> lock(this->groups_mutex_);
  auto groups = std::vector<Group>();
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

void Store::createStageMember(const nlohmann::json& payload)
{
  std::lock_guard<std::recursive_mutex> lock(this->stageMembers_mutex_);
  const auto _id = payload.at("_id").get<std::string>();
  stageMembers_[_id] = payload;
  const auto stageId = payload.at("stageId").get<std::string>();
  const auto groupId = payload.at("groupId").get<std::string>();
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
  std::lock_guard<std::recursive_mutex> lock(this->stageMembers_mutex_);
  const auto stageId = this->stageMembers_.at(id)["stageId"].get<std::string>();
  const auto groupId = this->stageMembers_.at(id)["groupId"].get<std::string>();
  this->stageMembers_.erase(id);
  stageMemberIds_by_stages_[stageId].erase(id);
  stageMemberIds_by_groups_[groupId].erase(id);
}

[[maybe_unused]] std::vector<StageMember>
Store::getStageMembersByStage(const std::string& stageId) const
{
  std::lock_guard<std::recursive_mutex> lock(this->stageMembers_mutex_);
  auto vector = std::vector<StageMember>();
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

std::vector<StageMember>
Store::getStageMembersByGroup(const std::string& groupId) const
{
  std::lock_guard<std::recursive_mutex> lock(this->stageMembers_mutex_);
  auto vector = std::vector<StageMember>();
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

std::optional<const CustomGroupPosition>
Store::getCustomGroupPositionByGroupAndDevice(const std::string& groupId,
                                              const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex> lock(this->customGroupPositions_mutex_);
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

void Store::createCustomGroupPosition(const json& payload)
{
  std::lock_guard<std::recursive_mutex> lock(this->customGroupPositions_mutex_);
  const auto _id = payload.at("_id").get<std::string>();
  customGroupPositions_[_id] = payload;
  const auto groupId = payload.at("groupId").get<std::string>();
  const auto deviceId = payload.at("deviceId").get<std::string>();
  if(customGroupPositionIds_by_Group_and_Device_.count(groupId) == 0) {
    customGroupPositionIds_by_Group_and_Device_[groupId] =
        std::map<std::string, std::string>();
  }
  customGroupPositionIds_by_Group_and_Device_[groupId][deviceId] = _id;
}

void Store::removeCustomGroupPosition(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(this->customGroupPositions_mutex_);
  const auto groupId =
      this->customGroupPositions_.at(id)["groupId"].get<std::string>();
  const auto deviceId =
      this->customGroupPositions_.at(id)["deviceId"].get<std::string>();
  customGroupPositionIds_by_Group_and_Device_[groupId].erase(deviceId);
  customGroupPositions_.erase(id);
}

std::optional<const CustomGroupVolume>
Store::getCustomGroupVolumeByGroupAndDevice(const std::string& groupId,
                                            const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex> lock(this->customGroupVolumes_mutex_);
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

void Store::createCustomGroupVolume(const json& payload)
{
  std::lock_guard<std::recursive_mutex> lock(this->customGroupVolumes_mutex_);
  const auto _id = payload.at("_id").get<std::string>();
  customGroupVolumes_[_id] = payload;
  const auto groupId = payload.at("groupId").get<std::string>();
  const auto deviceId = payload.at("deviceId").get<std::string>();
  if(customGroupVolumeIds_by_Group_and_Device_.count(groupId) == 0) {
    customGroupVolumeIds_by_Group_and_Device_[groupId] =
        std::map<std::string, std::string>();
  }
  customGroupVolumeIds_by_Group_and_Device_[groupId][deviceId] = _id;
}

void Store::removeCustomGroupVolume(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(this->customGroupVolumes_mutex_);
  const auto groupId =
      this->customGroupVolumes_.at(id)["groupId"].get<std::string>();
  const auto deviceId =
      this->customStageMemberVolumes_.at(id)["deviceId"].get<std::string>();
  customGroupVolumeIds_by_Group_and_Device_[groupId].erase(deviceId);
  customGroupVolumes_.erase(id);
}

[[maybe_unused]] std::optional<const CustomStageMemberPosition>
Store::getCustomStageMemberPositionByStageMemberAndDevice(
    const std::string& stageMemberId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex> lock(
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

void Store::createCustomStageMemberPosition(const json& payload)
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customStageMemberPositions_mutex_);
  const auto _id = payload.at("_id").get<std::string>();
  customStageMemberPositions_[_id] = payload;
  const auto stageMemberId = payload.at("stageMemberId").get<std::string>();
  const auto deviceId = payload.at("deviceId").get<std::string>();
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
  std::lock_guard<std::recursive_mutex> lock(
      this->customStageMemberPositions_mutex_);
  const auto stageMemberId =
      this->customStageMemberPositions_.at(id)["stageMemberId"]
          .get<std::string>();
  const auto deviceId =
      this->customStageMemberPositions_.at(id)["deviceId"].get<std::string>();
  customStageMemberPositionIds_by_StageMember_and_Device_[stageMemberId].erase(
      deviceId);
  customStageMemberPositions_.erase(id);
}

std::optional<const CustomStageMemberVolume>
Store::getCustomStageMemberVolumeByStageMemberAndDevice(
    const std::string& stageMemberId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customStageMemberVolumes_mutex_);
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

void Store::createCustomStageMemberVolume(const json& payload)
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customStageMemberVolumes_mutex_);
  const auto _id = payload.at("_id").get<std::string>();
  customStageMemberVolumes_[_id] = payload;
  const auto stageMemberId = payload.at("stageMemberId").get<std::string>();
  const auto deviceId = payload.at("deviceId").get<std::string>();
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
  std::lock_guard<std::recursive_mutex> lock(
      this->customStageMemberVolumes_mutex_);
  const auto stageMemberId =
      this->customStageMemberVolumes_.at(id)["stageMemberId"]
          .get<std::string>();
  const auto deviceId =
      this->customStageMemberVolumes_.at(id)["deviceId"].get<std::string>();
  customStageMemberVolumeIds_by_StageMember_and_Device_[stageMemberId].erase(
      deviceId);
  customStageMemberVolumes_.erase(id);
}

std::optional<const CustomAudioTrackPosition>
Store::getCustomAudioTrackPositionByAudioTrackAndDevice(
    const std::string& audioTrackId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customAudioTrackPositions_mutex_);
  if(this->customAudioTrackPositionIds_by_AudioTrack_and_Device_
         .count(audioTrackId) > 0) {
    if(this->customAudioTrackPositionIds_by_AudioTrack_and_Device_
           .at(audioTrackId)
           .count(deviceId) > 0) {
      return getCustomAudioTrackPosition(
          this->customAudioTrackPositionIds_by_AudioTrack_and_Device_
              .at(audioTrackId)
              .at(deviceId));
    }
  }
  return std::nullopt;
}

void Store::createCustomAudioTrackPosition(const json& payload)
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customAudioTrackPositions_mutex_);
  const auto _id = payload.at("_id").get<std::string>();
  customAudioTrackPositions_[_id] = payload;
  const auto audioTrackId =
      payload.at("audioTrackId").get<std::string>();
  const auto deviceId = payload.at("deviceId").get<std::string>();
  if(customAudioTrackPositionIds_by_AudioTrack_and_Device_.count(
         audioTrackId) == 0) {
    customAudioTrackPositionIds_by_AudioTrack_and_Device_
        [audioTrackId] = std::map<std::string, std::string>();
  }
  customAudioTrackPositionIds_by_AudioTrack_and_Device_
      [audioTrackId][deviceId] = _id;
}

void Store::removeCustomAudioTrackPosition(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customAudioTrackPositions_mutex_);
  const auto audioTrackId =
      this->customAudioTrackPositions_.at(id)["audioTrackId"]
          .get<std::string>();
  const auto deviceId =
      this->customAudioTrackPositions_.at(id)["audioTrackId"]
          .get<std::string>();
  customAudioTrackPositionIds_by_AudioTrack_and_Device_
      [audioTrackId]
          .erase(deviceId);
  customAudioTrackPositions_.erase(id);
}

[[maybe_unused]] std::optional<const CustomAudioTrackVolume>
Store::getCustomAudioTrackVolumeByAudioTrackAndDevice(
    const std::string& audioTrackId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customAudioTrackVolumes_mutex_);
  if(this->customAudioTrackVolumeIds_by_AudioTrack_and_Device_
         .count(audioTrackId) > 0) {
    if(this->customAudioTrackVolumeIds_by_AudioTrack_and_Device_
           .at(audioTrackId)
           .count(deviceId) > 0) {
      return getCustomAudioTrackVolume(
          this->customAudioTrackVolumeIds_by_AudioTrack_and_Device_
              .at(audioTrackId)
              .at(deviceId));
    }
  }
  return std::nullopt;
}

void Store::createCustomAudioTrackVolume(const json& payload)
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customAudioTrackVolumes_mutex_);
  const auto _id = payload.at("_id").get<std::string>();
  customAudioTrackVolumes_[_id] = payload;
  const auto audioTrackId =
      payload.at("audioTrackId").get<std::string>();
  const auto deviceId = payload.at("deviceId").get<std::string>();
  if(customAudioTrackVolumeIds_by_AudioTrack_and_Device_.count(
         audioTrackId) == 0) {
    customAudioTrackVolumeIds_by_AudioTrack_and_Device_
        [audioTrackId] = std::map<std::string, std::string>();
  }
  customAudioTrackVolumeIds_by_AudioTrack_and_Device_
      [audioTrackId][deviceId] = _id;
}

void Store::removeCustomAudioTrackVolume(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customAudioTrackVolumes_mutex_);
  const auto audioTrackId =
      this->customAudioTrackVolumes_.at(id)["audioTrackId"]
          .get<std::string>();
  const auto deviceId = this->customAudioTrackVolumes_.at(id)["deviceId"]
                            .get<std::string>();
  customAudioTrackVolumeIds_by_AudioTrack_and_Device_
      [audioTrackId]
          .erase(deviceId);
  customAudioTrackVolumes_.erase(id);
}

std::vector<VideoTrack>
Store::getVideoTracksByStageDevice(const std::string& stageDeviceId) const
{
  std::lock_guard<std::recursive_mutex> lock(this->videoTracks_mutex_);
  auto items = std::vector<VideoTrack>();
  if(this->videoTrackIds_by_StageDevice_.count(stageDeviceId) > 0) {
    auto ids = this->videoTrackIds_by_StageDevice_.at(stageDeviceId);
    for(const auto& id : ids) {
      auto group = getVideoTrack(id);
      if(group) {
        items.push_back(*group);
      }
    }
  }
  return items;
}
void Store::createVideoTrack(const json& payload)
{
  std::lock_guard<std::recursive_mutex> lock(this->videoTracks_mutex_);
  const auto _id = payload.at("_id").get<std::string>();
  videoTracks_[_id] = payload;
  const auto stageDeviceId = payload.at("stageDeviceId").get<std::string>();
  if(videoTrackIds_by_StageDevice_.count(stageDeviceId) == 0) {
    videoTrackIds_by_StageDevice_[stageDeviceId] =
        std::set<std::string>();
  }
  videoTrackIds_by_StageDevice_[stageDeviceId].insert(_id);
}
void Store::removeVideoTrack(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(this->videoTracks_mutex_);
  const auto stageDeviceId =
      this->videoTracks_.at(id)["stageDeviceId"].get<std::string>();
  this->videoTracks_.erase(id);
  videoTrackIds_by_StageDevice_[stageDeviceId].erase(id);
}

std::vector<AudioTrack>
Store::getAudioTracksByStageDevice(const std::string& stageDeviceId) const
{
  std::lock_guard<std::recursive_mutex> lock(this->audioTracks_mutex_);
  auto items = std::vector<AudioTrack>();
  if(this->audioTrackIds_by_StageDevice_.count(stageDeviceId) > 0) {
    auto ids = this->audioTrackIds_by_StageDevice_.at(stageDeviceId);
    for(const auto& id : ids) {
      auto group = getAudioTrack(id);
      if(group) {
        items.push_back(*group);
      }
    }
  }
  return items;
}
void Store::createAudioTrack(const json& payload)
{
  std::lock_guard<std::recursive_mutex> lock(this->audioTracks_mutex_);
  const auto _id = payload.at("_id").get<std::string>();
  audioTracks_[_id] = payload;
  const auto stageDeviceId = payload.at("stageDeviceId").get<std::string>();
  if(audioTrackIds_by_StageDevice_.count(stageDeviceId) == 0) {
    audioTrackIds_by_StageDevice_[stageDeviceId] =
        std::set<std::string>();
  }
  audioTrackIds_by_StageDevice_[stageDeviceId].insert(_id);
}
void Store::removeAudioTrack(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(this->audioTracks_mutex_);
  const auto stageDeviceId =
      this->audioTracks_.at(id)["stageDeviceId"].get<std::string>();
  this->audioTracks_.erase(id);
  audioTrackIds_by_StageDevice_[stageDeviceId].erase(id);
}

void Store::setReady(bool ready)
{
  std::lock_guard<std::recursive_mutex> lock(this->ready_mutex_);
  this->isReady_ = ready;
}

bool Store::isReady() const
{
  std::lock_guard<std::recursive_mutex> lock(this->ready_mutex_);
  return this->isReady_;
}

std::optional<DigitalStage::Types::SoundCard>
Store::getSoundCardByUUID(const std::string& uuid) const
{
  std::lock_guard<std::recursive_mutex> lock(this->mutex_soundCards_);
  for(auto& pair : this->soundCards_) {
    if(pair.second["uuid"] == uuid)
      return pair.second;
  }
  return std::nullopt;
}

void Store::createStageDevice(const json& payload)
{
  std::lock_guard<std::recursive_mutex> lock(this->stageDevices_mutex_);
  const auto _id = payload.at("_id").get<std::string>();
  stageDevices_[_id] = payload;
  const auto stageMemberId = payload.at("stageMemberId").get<std::string>();
  if(stageDeviceIds_by_stageMember_.count(stageMemberId) == 0) {
    stageDeviceIds_by_stageMember_[stageMemberId] = std::set<std::string>();
  }
  stageDeviceIds_by_stageMember_[stageMemberId].insert(_id);
}

std::vector<DigitalStage::Types::StageDevice>
Store::getStageDevicesByStageMember(const std::string& stageMemberId) const
{
  std::lock_guard<std::recursive_mutex> lock(this->stageDevices_mutex_);
  auto vector = std::vector<StageDevice>();
  if(this->stageDeviceIds_by_stageMember_.count(stageMemberId) > 0) {
    auto stageDeviceIds =
        this->stageDeviceIds_by_stageMember_.at(stageMemberId);
    for(const auto& stageDeviceId : stageDeviceIds) {
      auto stageDevice = getStageDevice(stageDeviceId);
      if(stageDevice) {
        vector.push_back(*stageDevice);
      }
    }
  }
  return vector;
}

void Store::removeStageDevice(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(this->stageDevices_mutex_);
  const auto stageMemberId =
      this->stageDevices_.at(id)["stageMemberId"].get<std::string>();
  this->stageMembers_.erase(id);
  stageDeviceIds_by_stageMember_[stageMemberId].erase(id);
}

void Store::createCustomStageDevicePosition(const json& payload)
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customStageDevicePositions_mutex_);
  const auto _id = payload.at("_id").get<std::string>();
  customStageDevicePositions_[_id] = payload;
  const auto stageDeviceId = payload.at("stageDeviceId").get<std::string>();
  const auto deviceId = payload.at("deviceId").get<std::string>();
  if(customStageDevicePositionIds_by_StageDevice_and_Device_.count(
         stageDeviceId) == 0) {
    customStageDevicePositionIds_by_StageDevice_and_Device_[stageDeviceId] =
        std::map<std::string, std::string>();
  }
  customStageDevicePositionIds_by_StageDevice_and_Device_[stageDeviceId]
                                                         [deviceId] = _id;
}

void Store::removeCustomStageDevicePosition(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customStageDevicePositions_mutex_);
  const auto stageDeviceId =
      this->customStageDevicePositions_.at(id)["stageDeviceId"]
          .get<std::string>();
  const auto deviceId =
      this->customStageDevicePositions_.at(id)["deviceId"].get<std::string>();
  customStageDevicePositionIds_by_StageDevice_and_Device_[stageDeviceId].erase(
      deviceId);
  customStageDevicePositions_.erase(id);
}

void Store::createCustomStageDeviceVolume(const json& payload)
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customStageDeviceVolumes_mutex_);
  const auto _id = payload.at("_id").get<std::string>();
  customStageDeviceVolumes_[_id] = payload;
  const auto stageDeviceId = payload.at("stageDeviceId").get<std::string>();
  const auto deviceId = payload.at("deviceId").get<std::string>();
  if(customStageDeviceVolumeIds_by_StageDevice_and_Device_.count(
         stageDeviceId) == 0) {
    customStageDeviceVolumeIds_by_StageDevice_and_Device_[stageDeviceId] =
        std::map<std::string, std::string>();
  }
  customStageDeviceVolumeIds_by_StageDevice_and_Device_[stageDeviceId]
                                                       [deviceId] = _id;
}

void Store::removeCustomStageDeviceVolume(const std::string& id)
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customStageDeviceVolumes_mutex_);
  const auto stageDeviceId =
      this->customStageDeviceVolumes_.at(id)["stageDeviceId"]
          .get<std::string>();
  const auto deviceId =
      this->customStageDeviceVolumes_.at(id)["deviceId"].get<std::string>();
  customStageDeviceVolumeIds_by_StageDevice_and_Device_[stageDeviceId].erase(
      deviceId);
  customStageDeviceVolumes_.erase(id);
}

std::optional<const CustomStageDeviceVolume>
Store::getCustomStageDeviceVolumeByStageDeviceAndDevice(
    const std::string& stageDeviceId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customStageDeviceVolumes_mutex_);
  if(this->customStageDeviceVolumeIds_by_StageDevice_and_Device_.count(
      stageDeviceId) > 0) {
    if(this->customStageDeviceVolumeIds_by_StageDevice_and_Device_
           .at(stageDeviceId)
           .count(deviceId) > 0) {
      return getCustomStageDeviceVolume(
          this->customStageDeviceVolumeIds_by_StageDevice_and_Device_
              .at(stageDeviceId)
              .at(deviceId));
    }
  }
  return std::nullopt;
}

std::optional<const CustomStageDevicePosition>
Store::getCustomStageDevicePositionByStageDeviceAndDevice(
    const std::string& stageDeviceId, const std::string& deviceId) const
{
  std::lock_guard<std::recursive_mutex> lock(
      this->customStageDevicePositions_mutex_);
  if(this->customStageDevicePositionIds_by_StageDevice_and_Device_.count(
      stageDeviceId) > 0) {
    if(this->customStageDevicePositionIds_by_StageDevice_and_Device_
           .at(stageDeviceId)
           .count(deviceId) > 0) {
      return getCustomStageDevicePosition(
          this->customStageDevicePositionIds_by_StageDevice_and_Device_
              .at(stageDeviceId)
              .at(deviceId));
    }
  }
  return std::nullopt;
}