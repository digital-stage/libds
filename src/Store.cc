#include "DigitalStage/Api/Store.h"
//#include <type_traits>  // for remove_reference<>::type

using namespace DigitalStage::Api;
using namespace DigitalStage::Types;

Store::Store()
    : isReady_(false) {}

std::optional<Device> Store::getLocalDevice() const {
  std::lock_guard<std::recursive_mutex> lock(this->local_device_id_mutex_);
  if (localDeviceId_) {
    return this->devices.get(*localDeviceId_);
  }
  return std::nullopt;
}

std::optional<ID_TYPE> Store::getLocalDeviceId() const {
  std::lock_guard<std::recursive_mutex> lock(this->local_device_id_mutex_);
  return localDeviceId_;
}

void Store::setLocalDeviceId(const ID_TYPE &id) {
  std::lock_guard<std::recursive_mutex> lock(this->local_device_id_mutex_);
  localDeviceId_ = id;
}

std::optional<ID_TYPE> Store::getStageDeviceId() const {
  std::lock_guard<std::recursive_mutex> lock(this->stage_device_id_mutex_);
  return stageDeviceId_;
}

void Store::setStageDeviceId(const ID_TYPE &id) {
  std::lock_guard<std::recursive_mutex> lock(this->stage_device_id_mutex_);
  stageDeviceId_ = id;
}

void Store::resetStageDeviceId() {
  std::lock_guard<std::recursive_mutex> lock(this->stage_device_id_mutex_);
  stageDeviceId_ = std::nullopt;
}

std::optional<StageDevice> Store::getStageDevice() const {
  std::lock_guard<std::recursive_mutex> lock(this->stage_device_id_mutex_);
  auto stageDeviceId = getStageDeviceId();
  if (stageDeviceId) {
    return stageDevices.get(*stageDeviceId);
  }
  return std::nullopt;
}

[[maybe_unused]] std::optional<ID_TYPE> Store::getStageMemberId() const {
  std::lock_guard<std::recursive_mutex> lock(this->stageMemberId_mutex_);
  return this->stageMemberId_;
}

void Store::setStageMemberId(const ID_TYPE &id) {
  std::lock_guard<std::recursive_mutex> lock(this->stageMemberId_mutex_);
  stageMemberId_ = id;
}

std::optional<ID_TYPE> Store::getUserId() const {
  std::lock_guard<std::recursive_mutex> lock(this->userId_mutex_);
  return userId_;
}

void Store::setUserId(const ID_TYPE &id) {
  std::lock_guard<std::recursive_mutex> lock(this->userId_mutex_);
  userId_ = id;
}

std::optional<ID_TYPE> Store::getStageId() const {
  std::lock_guard<std::recursive_mutex> lock(this->stageId_mutex_);
  return stageId_;
}

std::optional<DigitalStage::Types::Stage> Store::getStage() const {
  std::lock_guard<std::recursive_mutex> lock(this->stageId_mutex_);
  if (stageId_) {
    return stages.get(*stageId_);
  }
  return std::nullopt;
}

void Store::setStageId(const ID_TYPE &id) {
  std::lock_guard<std::recursive_mutex> lock(this->stageId_mutex_);
  stageId_ = id;
}

void Store::resetStageId() {
  std::lock_guard<std::recursive_mutex> lock(this->stageId_mutex_);
  stageId_ = std::nullopt;
}

std::optional<ID_TYPE> Store::getGroupId() const {
  std::lock_guard<std::recursive_mutex> lock(this->groupId_mutex_);
  return groupId_;
}

void Store::setGroupId(std::optional<ID_TYPE> id) {
  std::lock_guard<std::recursive_mutex> lock(this->groupId_mutex_);
  groupId_ = id;
}

void Store::resetGroupId() {
  std::lock_guard<std::recursive_mutex> lock(this->groupId_mutex_);
  groupId_ = std::nullopt;
}

std::vector<Group> Store::getGroupsByStage(const ID_TYPE &stageId) const {
  std::lock_guard<std::recursive_mutex> lock(this->groups.mutex_store_);
  auto vector = std::vector<Group>();
  for (const auto &item: this->groups.getAll()) {
    if (item.stageId == stageId) {
      vector.push_back(item);
    }
  }
  return vector;
}

[[maybe_unused]] std::vector<StageMember>
Store::getStageMembersByStage(const ID_TYPE &stageId) const {
  std::lock_guard<std::recursive_mutex> lock(this->stageMembers.mutex_store_);
  auto vector = std::vector<StageMember>();
  for (const auto &item: this->stageMembers.getAll()) {
    if (item.stageId == stageId) {
      vector.push_back(item);
    }
  }
  return vector;
}

std::vector<StageMember>
Store::getStageMembersByGroup(const ID_TYPE &groupId) const {
  std::lock_guard<std::recursive_mutex> lock(this->stageMembers.mutex_store_);
  auto vector = std::vector<StageMember>();
  for (const auto &item: this->stageMembers.getAll()) {
    if (item.groupId == groupId) {
      vector.push_back(item);
    }
  }
  return vector;
}

std::optional<CustomGroup>
Store::getCustomGroupByGroupAndTargetGroup(const ID_TYPE &groupId,
                                              const ID_TYPE &targetGroupId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customGroups.mutex_store_);
  for (const auto &item: this->customGroups.getAll()) {
    if (item.groupId == groupId && item.targetGroupId == targetGroupId) {
      return item;
    }
  }
  return std::nullopt;
}

std::optional<CustomGroupPosition>
Store::getCustomGroupPositionByGroupAndDevice(const ID_TYPE &groupId,
                                              const ID_TYPE &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customGroupPositions.mutex_store_);
  for (const auto &item: this->customGroupPositions.getAll()) {
    if (item.groupId == groupId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

std::optional<CustomGroupVolume>
Store::getCustomGroupVolumeByGroupAndDevice(const ID_TYPE &groupId,
                                            const ID_TYPE &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customGroupVolumes.mutex_store_);
  for (const auto &item: this->customGroupVolumes.getAll()) {
    if (item.groupId == groupId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

[[maybe_unused]] std::optional<CustomStageMemberPosition>
Store::getCustomStageMemberPositionByStageMemberAndDevice(
    const ID_TYPE &stageMemberId, const ID_TYPE &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customStageMemberPositions.mutex_store_);
  for (const auto &item: this->customStageMemberPositions.getAll()) {
    if (item.stageMemberId == stageMemberId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

std::optional<CustomStageMemberVolume>
Store::getCustomStageMemberVolumeByStageMemberAndDevice(
    const ID_TYPE &stageMemberId, const ID_TYPE &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customStageMemberVolumes.mutex_store_);
  for (const auto &item: this->customStageMemberVolumes.getAll()) {
    if (item.stageMemberId == stageMemberId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

std::optional<CustomAudioTrackPosition>
Store::getCustomAudioTrackPositionByAudioTrackAndDevice(
    const ID_TYPE &audioTrackId, const ID_TYPE &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customAudioTrackPositions.mutex_store_);
  for (const auto &item: this->customAudioTrackPositions.getAll()) {
    if (item.audioTrackId == audioTrackId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

[[maybe_unused]] std::optional<CustomAudioTrackVolume>
Store::getCustomAudioTrackVolumeByAudioTrackAndDevice(
    const ID_TYPE &audioTrackId, const ID_TYPE &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customAudioTrackVolumes.mutex_store_);
  for (const auto &item: this->customAudioTrackVolumes.getAll()) {
    if (item.audioTrackId == audioTrackId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

std::vector<VideoTrack>
Store::getVideoTracksByStageDevice(const ID_TYPE &stageDeviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->videoTracks.mutex_store_);
  auto vector = std::vector<VideoTrack>();
  for (const auto &item: this->videoTracks.getAll()) {
    if (item.stageDeviceId == stageDeviceId) {
      vector.push_back(item);
    }
  }
  return vector;
}

std::vector<AudioTrack>
Store::getAudioTracksByStageDevice(const ID_TYPE &stageDeviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->audioTracks.mutex_store_);
  auto vector = std::vector<AudioTrack>();
  for (const auto &item: this->audioTracks.getAll()) {
    if (item.stageDeviceId == stageDeviceId) {
      vector.push_back(item);
    }
  }
  return vector;
}

std::vector<AudioTrack>
Store::getAudioTracksByStageMember(const ID_TYPE &stageMemberId) const {
  std::lock_guard<std::recursive_mutex> lock(this->audioTracks.mutex_store_);
  auto vector = std::vector<AudioTrack>();
  for (const auto &audioTrack: this->audioTracks.getAll()) {
    if (audioTrack.stageMemberId == stageMemberId) {
      vector.push_back(audioTrack);
    }
  }
  return vector;
}

std::vector<AudioTrack>
Store::getAudioTracksByGroup(const ID_TYPE &groupId) const {
  auto vector = std::vector<AudioTrack>();
  for (const auto &stageMember: this->getStageMembersByGroup(groupId)) {
    auto audio_tracks = getAudioTracksByStageMember(stageMember._id);
    vector.insert(vector.end(), audio_tracks.begin(), audio_tracks.end());
  }
  return vector;
}

void Store::setReady(bool ready) {
  std::lock_guard<std::recursive_mutex> lock(this->ready_mutex_);
  this->isReady_ = ready;
}

bool Store::isReady() const {
  std::lock_guard<std::recursive_mutex> lock(this->ready_mutex_);
  return this->isReady_;
}

std::optional<DigitalStage::Types::SoundCard>
Store::getSoundCardByDeviceAndDriverAndTypeAndLabel(const ID_TYPE &deviceId,
                                                    const std::string &audioDriver,
                                                    const std::string &type,
                                                    const std::string &label) const {
  std::lock_guard<std::recursive_mutex> lock(this->soundCards.mutex_store_);
  for (const auto &item: this->soundCards.getAll()) {
    if (item.deviceId == deviceId &&
        item.audioDriver == audioDriver &&
        item.type == type &&
        item.label == label) {
      return item;
    }
  }
  return std::nullopt;
}

std::vector<DigitalStage::Types::StageDevice>
Store::getStageDevicesByStageMember(const ID_TYPE &stageMemberId) const {
  std::lock_guard<std::recursive_mutex> lock(this->stageDevices.mutex_store_);
  auto vector = std::vector<StageDevice>();
  for (const auto &item: this->stageDevices.getAll()) {
    if (item.stageMemberId == stageMemberId) {
      vector.push_back(item);
    }
  }
  return vector;
}

std::optional<CustomStageDeviceVolume>
Store::getCustomStageDeviceVolumeByStageDeviceAndDevice(
    const ID_TYPE &stageDeviceId, const ID_TYPE &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customStageDeviceVolumes.mutex_store_);
  for (const auto &item: this->customStageDeviceVolumes.getAll()) {
    if (item.stageDeviceId == stageDeviceId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

std::optional<CustomStageDevicePosition>
Store::getCustomStageDevicePositionByStageDeviceAndDevice(
    const ID_TYPE &stageDeviceId, const ID_TYPE &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customStageDevicePositions.mutex_store_);
  for (const auto &item: this->customStageDevicePositions.getAll()) {
    if (item.stageDeviceId == stageDeviceId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

std::optional<DigitalStage::Types::SoundCard> Store::getInputSoundCard() const {
  std::lock_guard<std::recursive_mutex> lock(this->soundCards.mutex_store_);
  auto localDevice = this->getLocalDevice();
  if (localDevice && localDevice->inputSoundCardId) {
    return this->soundCards.get(*localDevice->inputSoundCardId);
  }
  return std::nullopt;
}

std::optional<DigitalStage::Types::SoundCard> Store::getOutputSoundCard() const {
  std::lock_guard<std::recursive_mutex> lock(this->soundCards.mutex_store_);
  auto localDevice = this->getLocalDevice();
  if (localDevice && localDevice->outputSoundCardId) {
    return this->soundCards.get(*localDevice->outputSoundCardId);
  }
  return std::nullopt;
}
std::vector<DigitalStage::Types::AudioTrack> Store::getLocalAudioTracks() const {
  std::lock_guard<std::recursive_mutex> lock(this->audioTracks.mutex_store_);
  auto vector = std::vector<AudioTrack>();
  auto localDeviceId = this->getLocalDeviceId();
  if (localDeviceId) {
    for (const auto &item: this->audioTracks.getAll()) {
      if (item.deviceId == *localDeviceId) {
        vector.push_back(item);
      }
    }
  }
  return vector;
}

std::vector<std::string> Store::getTurnServers() const {
  std::lock_guard<std::recursive_mutex> lock(this->turn_mutex_);
  return turn_urls_;
}

void Store::setTurnServers(std::vector<std::string> turn_servers) {
  std::lock_guard<std::recursive_mutex> lock(this->turn_mutex_);
  turn_urls_ = turn_servers;
}

std::optional<std::string> Store::getTurnUsername() const {
  std::lock_guard<std::recursive_mutex> lock(this->turn_mutex_);
  return turn_username_;
}
std::optional<std::string> Store::getTurnPassword() const {
  std::lock_guard<std::recursive_mutex> lock(this->turn_mutex_);
  return turn_password_;
}

void Store::setTurnUsername(const std::string &username) {
  std::lock_guard<std::recursive_mutex> lock(this->turn_mutex_);
  turn_username_ = username;
}

void Store::setTurnPassword(const std::string &password) {
  std::lock_guard<std::recursive_mutex> lock(this->turn_mutex_);
  turn_password_ = password;
}