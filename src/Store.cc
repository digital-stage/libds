
#include "DigitalStage/Api/Store.h"

using namespace DigitalStage::Api;
using namespace DigitalStage::Types;

Store::Store() : isReady_(false) {}

std::optional<const Device> Store::getLocalDevice() const {
  std::lock_guard<std::recursive_mutex> lock(this->local_device_id_mutex_);
  if (localDeviceId_) {
    return this->devices.get(*localDeviceId_);
  }
  return std::nullopt;
}

std::optional<std::string> Store::getLocalDeviceId() const {
  std::lock_guard<std::recursive_mutex> lock(this->local_device_id_mutex_);
  return localDeviceId_;
}

void Store::setLocalDeviceId(const std::string &id) {
  std::lock_guard<std::recursive_mutex> lock(this->local_device_id_mutex_);
  localDeviceId_ = id;
}

std::optional<std::string> Store::getStageDeviceId() const {
  std::lock_guard<std::recursive_mutex> lock(this->stage_device_id_mutex_);
  return stageDeviceId_;
}

void Store::setStageDeviceId(const std::string &id) {
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

[[maybe_unused]] std::optional<std::string> Store::getStageMemberId() const {
  std::lock_guard<std::recursive_mutex> lock(this->stageMemberId_mutex_);
  return this->stageMemberId_;
}

void Store::setStageMemberId(const std::string &id) {
  std::lock_guard<std::recursive_mutex> lock(this->stageMemberId_mutex_);
  stageMemberId_ = id;
}

std::optional<std::string> Store::getUserId() const {
  std::lock_guard<std::recursive_mutex> lock(this->userId_mutex_);
  return userId_;
}

void Store::setUserId(const std::string &id) {
  std::lock_guard<std::recursive_mutex> lock(this->userId_mutex_);
  userId_ = id;
}

std::optional<std::string> Store::getStageId() const {
  std::lock_guard<std::recursive_mutex> lock(this->stageId_mutex_);
  return stageId_;
}

void Store::setStageId(const std::string &id) {
  std::lock_guard<std::recursive_mutex> lock(this->stageId_mutex_);
  stageId_ = id;
}

void Store::resetStageId() {
  std::lock_guard<std::recursive_mutex> lock(this->stageId_mutex_);
  stageId_ = std::nullopt;
}

std::optional<std::string> Store::getGroupId() const {
  std::lock_guard<std::recursive_mutex> lock(this->groupId_mutex_);
  return groupId_;
}

void Store::setGroupId(const std::string &id) {
  std::lock_guard<std::recursive_mutex> lock(this->groupId_mutex_);
  groupId_ = id;
}

void Store::resetGroupId() {
  std::lock_guard<std::recursive_mutex> lock(this->groupId_mutex_);
  groupId_ = std::nullopt;
}

std::vector<Group> Store::getGroupsByStage(const std::string &stageId) const {
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
Store::getStageMembersByStage(const std::string &stageId) const {
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
Store::getStageMembersByGroup(const std::string &groupId) const {
  std::lock_guard<std::recursive_mutex> lock(this->stageMembers.mutex_store_);
  auto vector = std::vector<StageMember>();
  for (const auto &item: this->stageMembers.getAll()) {
    if (item.groupId == groupId) {
      vector.push_back(item);
    }
  }
  return vector;
}

std::optional<const CustomGroupPosition>
Store::getCustomGroupPositionByGroupAndDevice(const std::string &groupId,
                                              const std::string &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customGroupPositions.mutex_store_);
  for (const auto &item: this->customGroupPositions.getAll()) {
    if (item.groupId == groupId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

std::optional<const CustomGroupVolume>
Store::getCustomGroupVolumeByGroupAndDevice(const std::string &groupId,
                                            const std::string &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customGroupVolumes.mutex_store_);
  for (const auto &item: this->customGroupVolumes.getAll()) {
    if (item.groupId == groupId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

[[maybe_unused]] std::optional<const CustomStageMemberPosition>
Store::getCustomStageMemberPositionByStageMemberAndDevice(
    const std::string &stageMemberId, const std::string &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customStageMemberPositions.mutex_store_);
  for (const auto &item: this->customStageMemberPositions.getAll()) {
    if (item.stageMemberId == stageMemberId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

std::optional<const CustomStageMemberVolume>
Store::getCustomStageMemberVolumeByStageMemberAndDevice(
    const std::string &stageMemberId, const std::string &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customStageMemberVolumes.mutex_store_);
  for (const auto &item: this->customStageMemberVolumes.getAll()) {
    if (item.stageMemberId == stageMemberId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

std::optional<const CustomAudioTrackPosition>
Store::getCustomAudioTrackPositionByAudioTrackAndDevice(
    const std::string &audioTrackId, const std::string &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customAudioTrackPositions.mutex_store_);
  for (const auto &item: this->customAudioTrackPositions.getAll()) {
    if (item.audioTrackId == audioTrackId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

[[maybe_unused]] std::optional<const CustomAudioTrackVolume>
Store::getCustomAudioTrackVolumeByAudioTrackAndDevice(
    const std::string &audioTrackId, const std::string &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customAudioTrackVolumes.mutex_store_);
  for (const auto &item: this->customAudioTrackVolumes.getAll()) {
    if (item.audioTrackId == audioTrackId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

std::vector<VideoTrack>
Store::getVideoTracksByStageDevice(const std::string &stageDeviceId) const {
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
Store::getAudioTracksByStageDevice(const std::string &stageDeviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->audioTracks.mutex_store_);
  auto vector = std::vector<AudioTrack>();
  for (const auto &item: this->audioTracks.getAll()) {
    if (item.stageDeviceId == stageDeviceId) {
      vector.push_back(item);
    }
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
Store::getSoundCardByDeviceAndDriverAndTypeAndLabel(const std::string &deviceId,
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
Store::getStageDevicesByStageMember(const std::string &stageMemberId) const {
  std::lock_guard<std::recursive_mutex> lock(this->stageDevices.mutex_store_);
  auto vector = std::vector<StageDevice>();
  for (const auto &item: this->stageDevices.getAll()) {
    if (item.stageMemberId == stageMemberId) {
      vector.push_back(item);
    }
  }
  return vector;
}

std::optional<const CustomStageDeviceVolume>
Store::getCustomStageDeviceVolumeByStageDeviceAndDevice(
    const std::string &stageDeviceId, const std::string &deviceId) const {
  std::lock_guard<std::recursive_mutex> lock(this->customStageDeviceVolumes.mutex_store_);
  for (const auto &item: this->customStageDeviceVolumes.getAll()) {
    if (item.stageDeviceId == stageDeviceId && item.deviceId == deviceId) {
      return item;
    }
  }
  return std::nullopt;
}

std::optional<const CustomStageDevicePosition>
Store::getCustomStageDevicePositionByStageDeviceAndDevice(
    const std::string &stageDeviceId, const std::string &deviceId) const {
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
