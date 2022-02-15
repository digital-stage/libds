
#include "DigitalStage/Api/Store.h"

using namespace DigitalStage::Api;
using namespace DigitalStage::Types;

Store::Store()
    : isReady_(false) {}

std::optional<Device> Store::getLocalDevice() const {
  if (localDeviceId_.has_value()) {
    return this->devices.get(*localDeviceId_.get());
  }
  return std::nullopt;
}

std::optional<ID_TYPE> Store::getLocalDeviceId() const {
  return localDeviceId_.get();
}

void Store::setLocalDeviceId(const ID_TYPE &id) {
  localDeviceId_ .set(id);
}

std::optional<ID_TYPE> Store::getStageDeviceId() const {
  return stageDeviceId_.get();
}

void Store::setStageDeviceId(const ID_TYPE &id) {
  stageDeviceId_.set(id);
}

void Store::resetStageDeviceId() {
  stageDeviceId_.set(std::nullopt);
}

std::optional<StageDevice> Store::getStageDevice() const {
  auto stageDeviceId = getStageDeviceId();
  if (stageDeviceId.has_value()) {
    return stageDevices.get(*stageDeviceId);
  }
  return std::nullopt;
}

[[maybe_unused]] std::optional<ID_TYPE> Store::getStageMemberId() const {
  return stageMemberId_.get();
}

void Store::setStageMemberId(const ID_TYPE &id) {
  stageMemberId_.set(id);
}

std::optional<ID_TYPE> Store::getUserId() const {
  return userId_.get();
}

void Store::setUserId(const ID_TYPE &id) {
  userId_.set(id);
}

std::optional<ID_TYPE> Store::getStageId() const {
  return stageId_.get();
}

std::optional<DigitalStage::Types::Stage> Store::getStage() const {
  if (stageId_.has_value()) {
    return stages.get(*stageId_.get());
  }
  return std::nullopt;
}

void Store::setStageId(const ID_TYPE &id) {
  stageId_.set(id);
}

void Store::resetStageId() {
  stageId_.set(std::nullopt);
}

std::optional<ID_TYPE> Store::getGroupId() const {
  return groupId_.get();
}

void Store::setGroupId(std::optional<ID_TYPE> id) {
  groupId_.set(id);
}

void Store::resetGroupId() {
  groupId_.set(std::nullopt);
}

void Store::resetStageMemberId() {
  stageMemberId_.set(std::nullopt);
}

std::vector<Group> Store::getGroupsByStage(const ID_TYPE &stageId) const {
  auto vector = std::vector<Group>();
  for (const auto &item: groups.getAll()) {
    if (item.stageId == stageId) {
      vector.push_back(item);
    }
  }
  return vector;
}

[[maybe_unused]] std::vector<StageMember>
Store::getStageMembersByStage(const ID_TYPE &stageId) const {
  auto vector = std::vector<StageMember>();
  for (const auto &item: stageMembers.getAll()) {
    if (item.stageId == stageId) {
      vector.push_back(item);
    }
  }
  return vector;
}

std::vector<StageMember>
Store::getStageMembersByGroup(const ID_TYPE &groupId) const {
  auto vector = std::vector<StageMember>();
  for (const auto &item: stageMembers.getAll()) {
    if (item.groupId == groupId) {
      vector.push_back(item);
    }
  }
  return vector;
}

std::optional<CustomGroup>
Store::getCustomGroupByGroupAndTargetGroup(const ID_TYPE &groupId,
                                              const ID_TYPE &targetGroupId) const {
  for (const auto &item: customGroups.getAll()) {
    if (item.groupId == groupId && item.targetGroupId == targetGroupId) {
      return item;
    }
  }
  return std::nullopt;
}

std::vector<VideoTrack>
Store::getVideoTracksByStageDevice(const ID_TYPE &stageDeviceId) const {
  auto vector = std::vector<VideoTrack>();
  for (const auto &item: videoTracks.getAll()) {
    if (item.stageDeviceId == stageDeviceId) {
      vector.push_back(item);
    }
  }
  return vector;
}

std::vector<AudioTrack>
Store::getAudioTracksByStageDevice(const ID_TYPE &stageDeviceId) const {
  auto vector = std::vector<AudioTrack>();
  for (const auto &item: audioTracks.getAll()) {
    if (item.stageDeviceId == stageDeviceId) {
      vector.push_back(item);
    }
  }
  return vector;
}

std::vector<AudioTrack>
Store::getAudioTracksByStageMember(const ID_TYPE &stageMemberId) const {
  auto vector = std::vector<AudioTrack>();
  for (const auto &audioTrack: audioTracks.getAll()) {
    if (audioTrack.stageMemberId == stageMemberId) {
      vector.push_back(audioTrack);
    }
  }
  return vector;
}

std::optional<DigitalStage::Types::AudioTrack>
Store::getAudioTrackByUuid(const ID_TYPE & uuid) const
{
    std::lock_guard<std::recursive_mutex> lock(this->audioTracks.mutex_store_);    
    for (const auto & audioTrack : this->audioTracks.getAll()) {
        if (audioTrack.uuid == uuid) {
            return audioTrack;
        }
    }
    return {};
}


std::vector<AudioTrack>
Store::getAudioTracksByGroup(const ID_TYPE &groupId) const {
  auto vector = std::vector<AudioTrack>();
  for (const auto &stageMember: getStageMembersByGroup(groupId)) {
    auto audio_tracks = getAudioTracksByStageMember(stageMember._id);
    vector.insert(vector.end(), audio_tracks.begin(), audio_tracks.end());
  }
  return vector;
}

void Store::setReady(bool ready) {
  this->isReady_ = ready;
}

bool Store::isReady() const {
  return this->isReady_;
}

std::optional<DigitalStage::Types::SoundCard>
Store::getSoundCardByDeviceAndDriverAndTypeAndLabel(const ID_TYPE &deviceId,
                                                    const std::string &audioDriver,
                                                    const std::string &type,
                                                    const std::string &label) const {
  for (const auto &item: soundCards.getAll()) {
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
  auto vector = std::vector<StageDevice>();
  for (const auto &item: stageDevices.getAll()) {
    if (item.stageMemberId == stageMemberId) {
      vector.push_back(item);
    }
  }
  return vector;
}

std::optional<DigitalStage::Types::SoundCard> Store::getInputSoundCard() const {
  auto localDevice = getLocalDevice();
  if (localDevice && localDevice->inputSoundCardId) {
    return soundCards.get(*localDevice->inputSoundCardId);
  }
  return std::nullopt;
}

std::optional<DigitalStage::Types::SoundCard> Store::getOutputSoundCard() const {
  auto localDevice = this->getLocalDevice();
  if (localDevice && localDevice->outputSoundCardId) {
    return this->soundCards.get(*localDevice->outputSoundCardId);
  }
  return std::nullopt;
}
std::vector<DigitalStage::Types::AudioTrack> Store::getLocalAudioTracks() const {
  auto vector = std::vector<AudioTrack>();
  auto localDeviceId = getLocalDeviceId();
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
  std::lock_guard<std::mutex> lock(turn_url_mutex_);
  return turn_urls_;
}

void Store::setTurnServers(std::vector<std::string> turn_servers) {
  std::lock_guard<std::mutex> lock(turn_url_mutex_);
  turn_urls_ = turn_servers;
}

std::optional<std::string> Store::getTurnUsername() const {
  return turn_username_.get();
}
std::optional<std::string> Store::getTurnPassword() const {
  return turn_password_.get();
}

void Store::setTurnUsername(const std::string &username) {
  turn_username_.set(username);
}

void Store::setTurnPassword(const std::string &password) {
  turn_password_.set(password);
}
