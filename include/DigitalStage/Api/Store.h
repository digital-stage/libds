
#ifndef DS_STORE
#define DS_STORE

#include "DigitalStage/Types.h"
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <optional>
#include <set>

using namespace nlohmann;

namespace DigitalStage::Api {

// Devices
template<typename TYPE>
class StoreEntry {
 public:
  std::optional<const TYPE> get(const std::string &id) const {
    std::lock_guard<std::recursive_mutex> lock(mutex_store_);
    if (storeEntry_.count(id) > 0) {
      return storeEntry_.at(id).template get<const TYPE>();
    }
    return std::nullopt;
  }

  const std::vector<TYPE> getAll() const {
    std::lock_guard<std::recursive_mutex> lock(mutex_store_);
    std::vector<TYPE> items = std::vector<TYPE>();
    for (const auto &item: storeEntry_) {
      items.push_back(item.second.template get<TYPE>());
    }
    return items;
  }

  void create(const json &payload) {
    std::lock_guard<std::recursive_mutex> lock(mutex_store_);
    const std::string _id = payload.at("_id").get<std::string>();
    storeEntry_[_id] = payload;
  }

  void update(const json &payload) {
    const std::string &id = payload.at("_id").get<std::string>();
    std::lock_guard<std::recursive_mutex> lock(mutex_store_);
    storeEntry_[id].merge_patch(payload);
  }

  void remove(const std::string &id) {
    std::lock_guard<std::recursive_mutex> lock(mutex_store_);
    storeEntry_.erase(id);
  }

  void removeAll() {
    std::lock_guard<std::recursive_mutex> lock(mutex_store_);
    storeEntry_.clear();
  }

 public:
  // TODO: @christofmuc to discuss if we should hide this mutex and use special mutex inside the helper functions (e.g. getGroupsByStage)
  mutable std::recursive_mutex mutex_store_;

 private:
  std::map<std::string, json> storeEntry_;
};

class Store {
 public:
  Store();

  void setReady(bool ready);
  /**
   * Signals, if the initial state has been fetched and the store is ready to use.
   * @return true if the store can be used, otherwise false
   */
  bool isReady() const;

  // Local device
  /**
   * Get this (local) device
   * @return this device
   */
  std::optional<const DigitalStage::Types::Device> getLocalDevice() const;
  void setLocalDeviceId(const std::string &id);
  /**
   * Returns the ID of this (local) device
   * @return
   */
  std::optional<std::string> getLocalDeviceId() const;

  // Devices
  StoreEntry<DigitalStage::Types::Device> devices;

  // Local user
  void setUserId(const std::string &id);
  /**
   * Returns the ID of the current user
   * @return ID of the current user
   */
  std::optional<std::string> getUserId() const;
  void setStageMemberId(const std::string &id);
  /**
   * Returns the ID of the current stage member
   * This will be null if the user is currently not inside any stage
   * @return ID of the current stage member or null
   */
  [[maybe_unused]] std::optional<std::string> getStageMemberId() const;

  // Users
  StoreEntry<DigitalStage::Types::User> users;

  /*
   * Stage management
   */
  void setStageId(const std::string &id);
  void resetStageId();
  /**
   * Returns the ID of the current stage.
   * This will return null if the user is currently not inside any stage.
   * Use this variable to obtain, if the user is currently inside a stage.
   * @return ID of the current stage or null
   */
  std::optional<std::string> getStageId() const;

  void setGroupId(const std::string &id);
  void resetGroupId();
  /**
   * Returns the ID of the current group.
   * This will return null if the user is currently not inside any stage.
   * @return ID of the current group or null
   */
  [[maybe_unused]] std::optional<std::string> getGroupId() const;

  void setStageDeviceId(const std::string &id);
  void resetStageDeviceId();
  /**
   * Returns the ID of this (local) stage device.
   * A stage device is an abstraction of this device for all stage members.
   * This will return null if the user is currently not inside any stage.
   * @return ID of this stage device or null
   */
  std::optional<std::string> getStageDeviceId() const;
  /**
   * Returns the this (local) stage device.
   * This will return null if the user is currently not inside any stage.
   * @return this stage device or null
   */
  std::optional<DigitalStage::Types::StageDevice> getStageDevice() const;

  // Stages
  StoreEntry<DigitalStage::Types::Stage> stages;

  // Groups
  StoreEntry<DigitalStage::Types::Group> groups;
  std::vector<DigitalStage::Types::Group> getGroupsByStage(const std::string &stageId) const;

  // Stage members
  StoreEntry<DigitalStage::Types::StageMember> stageMembers;
  [[maybe_unused]] std::vector<DigitalStage::Types::StageMember>
  getStageMembersByStage(const std::string &stageId) const;
  std::vector<DigitalStage::Types::StageMember>
  getStageMembersByGroup(const std::string &audio_trackId) const;

  // Stage devices
  StoreEntry<DigitalStage::Types::StageDevice> stageDevices;
  std::vector<DigitalStage::Types::StageDevice>
  getStageDevicesByStageMember(const std::string &stageMemberId) const;

  // Video tracks
  StoreEntry<DigitalStage::Types::VideoTrack> videoTracks;
  std::vector<DigitalStage::Types::VideoTrack>
  getVideoTracksByStageDevice(const std::string &stageDeviceId) const;

  // Audio tracks
  StoreEntry<DigitalStage::Types::AudioTrack> audioTracks;
  std::vector<DigitalStage::Types::AudioTrack>
  getAudioTracksByStageDevice(const std::string &stageDeviceId) const;
  std::vector<DigitalStage::Types::AudioTrack> getLocalAudioTracks() const;

  // Custom group positions
  StoreEntry<DigitalStage::Types::CustomGroupPosition> customGroupPositions;
  std::optional<const DigitalStage::Types::CustomGroupPosition>
  getCustomGroupPositionByGroupAndDevice(const std::string &groupId,
                                         const std::string &deviceId) const;

  // Custom group volumes
  StoreEntry<DigitalStage::Types::CustomGroupVolume> customGroupVolumes;
  std::optional<const DigitalStage::Types::CustomGroupVolume>
  getCustomGroupVolumeByGroupAndDevice(const std::string &groupId,
                                       const std::string &deviceId) const;

  // Custom stage member positions
  StoreEntry<DigitalStage::Types::CustomStageMemberPosition> customStageMemberPositions;
  [[maybe_unused]] std::optional<const DigitalStage::Types::CustomStageMemberPosition>
  getCustomStageMemberPositionByStageMemberAndDevice(
      const std::string &stageMemberId, const std::string &deviceId) const;

  // Custom stage member volumes
  StoreEntry<DigitalStage::Types::CustomStageMemberVolume> customStageMemberVolumes;
  [[maybe_unused]] std::optional<const DigitalStage::Types::CustomStageMemberVolume>
  getCustomStageMemberVolumeByStageMemberAndDevice(
      const std::string &stageMemberId, const std::string &deviceId) const;

  // Custom stage device positions
  StoreEntry<DigitalStage::Types::CustomStageDevicePosition> customStageDevicePositions;
  [[maybe_unused]] std::optional<const DigitalStage::Types::CustomStageDevicePosition>
  getCustomStageDevicePositionByStageDeviceAndDevice(
      const std::string &stageDeviceId, const std::string &deviceId) const;

  // Custom stage device volumes
  StoreEntry<DigitalStage::Types::CustomStageDeviceVolume> customStageDeviceVolumes;
  [[maybe_unused]] std::optional<const DigitalStage::Types::CustomStageDeviceVolume>
  getCustomStageDeviceVolumeByStageDeviceAndDevice(
      const std::string &stageDeviceId, const std::string &deviceId) const;

  // Custom audio track positions
  StoreEntry<DigitalStage::Types::CustomAudioTrackPosition> customAudioTrackPositions;
  [[maybe_unused]] std::optional<const DigitalStage::Types::CustomAudioTrackPosition>
  getCustomAudioTrackPositionByAudioTrackAndDevice(
      const std::string &audio_trackId,
      const std::string &deviceId) const;

  // Custom audio track volumes
  StoreEntry<DigitalStage::Types::CustomAudioTrackVolume> customAudioTrackVolumes;
  [[maybe_unused]] std::optional<const DigitalStage::Types::CustomAudioTrackVolume>
  getCustomAudioTrackVolumeByAudioTrackAndDevice(
      const std::string &audio_trackId,
      const std::string &deviceId) const;

  // Sound cards
  StoreEntry<DigitalStage::Types::SoundCard> soundCards;
  std::optional<DigitalStage::Types::SoundCard>
  getSoundCardByDeviceAndDriverAndTypeAndLabel(const std::string &deviceId,
                                               const std::string &audioDriver,
                                               const std::string &type,
                                               const std::string &label) const;
  /**
   * Returns the current input sound card for this device if set
   * @return current input sound card
   */
  std::optional<DigitalStage::Types::SoundCard> getInputSoundCard() const;
  /**
   * Returns the current output sound card for this device if set
   * @return current output sound card
   */
  std::optional<DigitalStage::Types::SoundCard> getOutputSoundCard() const;

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
};
} // namespace DigitalStage::Api

#endif // DS_STORE