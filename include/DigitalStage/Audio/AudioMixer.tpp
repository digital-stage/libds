//
// Created by Tobias Hegemann on 04.11.21.
//

using namespace DigitalStage::Audio;

template<class T>
AudioMixer<T>::AudioMixer(std::shared_ptr<DigitalStage::Api::Client> client, bool use_balance)
    : client_(std::move(client)),
      token_(std::make_shared<DigitalStage::Api::Client::Token>()),
      use_balance_(use_balance) {
  attachHandlers();
}

template<class T>
void AudioMixer<T>::applyGain(const std::string &audio_track_id,
                              T *data,
                              std::size_t frame_count) {
  if (volume_map_.count(audio_track_id)) {
    auto item = volume_map_[audio_track_id];
    double volume = item.second ? 0 : item.first; // if muted use 0 as volume
    for (int frame = 0; frame < frame_count; frame++) {
      data[frame] = data[frame] * volume;
    }
  }
}

template<class T>
T AudioMixer<T>::applyGain(const std::string &audio_track_id, T data) {
  if (volume_map_.count(audio_track_id)) {
    auto item = volume_map_[audio_track_id];
    return item.second ? 0 : data * (item.first);
  }
  return data;
}

template<class T>
void AudioMixer<T>::attachHandlers() {
  // React to all changes of volume related entities and precalculate the resulting volume
  client_->ready.connect([this](const DigitalStage::Api::Store *store) {
    auto audioTracks = store->audioTracks.getAll();
    for (const auto &audio_track: audioTracks) {
      volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
    }
  }, token_);
  client_->audioTrackAdded.connect([this](const AudioTrack &audio_track, const DigitalStage::Api::Store *store) {
    volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
  }, token_);
  client_->audioTrackChanged.connect([this](const std::string &audio_track_id, const nlohmann::json &update,
                                            const DigitalStage::Api::Store *store) {
    if (update.contains("volume") || update.contains("muted")) {
      auto audio_track = store->audioTracks.get(audio_track_id);
      assert(audio_track);
      volume_map_[audio_track->_id] = calculateVolume(*audio_track, *store);
    }
  }, token_);
  client_->audioTrackRemoved.connect([this](const AudioTrack &audio_track, const DigitalStage::Api::Store *) {
    volume_map_.erase(audio_track._id);
  }, token_);
  client_->customAudioTrackVolumeAdded.connect([this](const CustomAudioTrackVolume &custom_volume,
                                                      const DigitalStage::Api::Store *store) {
    auto audio_track = store->audioTracks.get(custom_volume.audioTrackId);
    assert(audio_track);
    volume_map_[custom_volume._id] = calculateVolume(*audio_track, *store);
  }, token_);
  client_->customAudioTrackVolumeChanged.connect([this](const std::string &id, const nlohmann::json &update,
                                                        const DigitalStage::Api::Store *store) {
    if (update.contains("volume") || update.contains("muted")) {
      auto custom_volume = store->customAudioTrackVolumes.get(id);
      assert(custom_volume);
      auto audio_track = store->audioTracks.get(custom_volume->audioTrackId);
      assert(audio_track);
      volume_map_[audio_track->_id] = calculateVolume(*audio_track, *store);
    }
  }, token_);
  client_->customAudioTrackVolumeRemoved.connect([this](const CustomAudioTrackVolume &custom_volume,
                                                        const DigitalStage::Api::Store *store) {
    auto audio_track = store->audioTracks.get(custom_volume.audioTrackId);
    assert(audio_track);
    volume_map_[audio_track->_id] = calculateVolume(*audio_track, *store);
  }, token_);
  client_->stageDeviceChanged.connect([this](const std::string &stage_device_id, const nlohmann::json &update,
                                             const DigitalStage::Api::Store *store) {
    if (update.contains("volume") || update.contains("muted")) {
      // Find and update all related audio tracks
      for (const auto &audio_track: store->audioTracks.getAll()) {
        if (audio_track.stageDeviceId == stage_device_id) {
          volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
        }
      }
    }
  }, token_);
  client_->customStageDeviceVolumeAdded.connect([this](const CustomStageDeviceVolume &custom_volume,
                                                       const DigitalStage::Api::Store *store) {
    for (const auto &audio_track: store->audioTracks.getAll()) {
      if (audio_track.stageDeviceId == custom_volume.stageDeviceId) {
        volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
      }
    }
  }, token_);
  client_->customStageDeviceVolumeChanged.connect([this](const std::string &custom_stage_device_volume_id,
                                                         const nlohmann::json &update,
                                                         const DigitalStage::Api::Store *store) {
    if (update.contains("volume") || update.contains("muted")) {
      auto custom_stage_device_volume = store->customStageDeviceVolumes.get(custom_stage_device_volume_id);
      assert(custom_stage_device_volume);
      // Find and update all related audio tracks
      for (const auto &audio_track: store->audioTracks.getAll()) {
        if (audio_track.stageDeviceId == custom_stage_device_volume->stageDeviceId) {
          volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
        }
      }
    }
  }, token_);
  client_->customStageDeviceVolumeRemoved.connect([this](const CustomStageDeviceVolume &custom_volume,
                                                         const DigitalStage::Api::Store *store) {
    for (const auto &audio_track: store->audioTracks.getAll()) {
      if (audio_track.stageDeviceId == custom_volume.stageDeviceId) {
        volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
      }
    }
  }, token_);
  client_->stageMemberChanged.connect([this](const std::string &stage_member_id, const nlohmann::json &update,
                                             const DigitalStage::Api::Store *store) {
    if (update.contains("volume") || update.contains("muted")) {
      // Find and update all related audio tracks
      for (const auto &audio_track: store->audioTracks.getAll()) {
        if (audio_track.stageMemberId == stage_member_id) {
          volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
        }
      }
    }
  }, token_);
  client_->customStageMemberVolumeAdded.connect([this](const CustomStageMemberVolume &custom_volume,
                                                       const DigitalStage::Api::Store *store) {
    for (const auto &audio_track: store->audioTracks.getAll()) {
      if (audio_track.stageMemberId == custom_volume.stageMemberId) {
        volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
      }
    }
  }, token_);
  client_->customStageMemberVolumeChanged.connect([this](const std::string &custom_stage_member_volume_id,
                                                         const nlohmann::json &update,
                                                         const DigitalStage::Api::Store *store) {
    if (update.contains("volume") || update.contains("muted")) {
      auto custom_stage_member_volume = store->customStageMemberVolumes.get(custom_stage_member_volume_id);
      assert(custom_stage_member_volume);
      // Find and update all related audio tracks
      for (const auto &audio_track: store->audioTracks.getAll()) {
        if (audio_track.stageMemberId == custom_stage_member_volume->stageMemberId) {
          volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
        }
      }
    }
  }, token_);
  client_->customStageMemberVolumeRemoved.connect([this](const CustomStageMemberVolume &custom_volume,
                                                         const DigitalStage::Api::Store *store) {
    for (const auto &audio_track: store->audioTracks.getAll()) {
      if (audio_track.stageMemberId == custom_volume.stageMemberId) {
        volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
      }
    }
  }, token_);
  client_->groupChanged.connect([this](const std::string &group_id, const nlohmann::json &update,
                                       const DigitalStage::Api::Store *store) {
    if (update.contains("volume") || update.contains("muted")) {
      // Find and update all related audio tracks
      for (const auto &stage_member: store->getStageMembersByGroup(group_id)) {
        for (const auto &audio_track: store->audioTracks.getAll()) {
          if (audio_track.stageMemberId == stage_member._id) {
            volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
          }
        }
      }
    }
  }, token_);
  client_->customGroupVolumeAdded.connect([this](const CustomGroupVolume &custom_volume,
                                                 const DigitalStage::Api::Store *store) {
    for (const auto &stage_member: store->getStageMembersByGroup(custom_volume.groupId)) {
      for (const auto &audio_track: store->audioTracks.getAll()) {
        if (audio_track.stageMemberId == stage_member._id) {
          volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
        }
      }
    }
  }, token_);
  client_->customGroupVolumeChanged.connect([this](const std::string &custom_group_volume_id,
                                                   const nlohmann::json &update,
                                                   const DigitalStage::Api::Store *store) {
    if (update.contains("volume") || update.contains("muted")) {
      auto custom_group_volume = store->customGroupVolumes.get(custom_group_volume_id);
      assert(custom_group_volume);
      // Find and update all related audio tracks
      for (const auto &stage_member: store->getStageMembersByGroup(custom_group_volume->groupId)) {
        for (const auto &audio_track: store->audioTracks.getAll()) {
          if (audio_track.stageMemberId == stage_member._id) {
            volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
          }
        }
      }
    }
  }, token_);
  client_->customGroupVolumeRemoved.connect([this](const CustomGroupVolume &custom_volume,
                                                   const DigitalStage::Api::Store *store) {
    for (const auto &stage_member: store->getStageMembersByGroup(custom_volume.groupId)) {
      for (const auto &audio_track: store->audioTracks.getAll()) {
        if (audio_track.stageMemberId == stage_member._id) {
          volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
        }
      }
    }
  }, token_);
}

template<class T>
double AudioMixer<T>::calculateBalance(double balance, bool is_local) {
  if (is_local) {
    return sqrtf(0.5f * (1.0f - balance));
  } else {
    return sqrtf(0.5f * (1.0f + balance));
  }
}

template<class T>
std::pair<T, bool> AudioMixer<T>::calculateVolume(const AudioTrack &audio_track,
                                                  const DigitalStage::Api::Store &store) {
  // Get this device ID
  auto local_device_id = store.getLocalDeviceId();
  assert(local_device_id);

  auto custom_audio_track_volume =
      store.getCustomAudioTrackVolumeByAudioTrackAndDevice(audio_track._id, *local_device_id);
  // Get related stage device
  auto stage_device = store.stageDevices.get(audio_track.stageDeviceId);
  assert(stage_device);
  auto custom_stage_device_volume =
      store.getCustomStageDeviceVolumeByStageDeviceAndDevice(stage_device->_id, *local_device_id);
  // Get related stage member
  auto stage_member = store.stageMembers.get(audio_track.stageMemberId);
  assert(stage_member);
  auto custom_stage_member_volume =
      store.getCustomStageMemberVolumeByStageMemberAndDevice(stage_member->_id, *local_device_id);

  // Get related group
  auto group = stage_member->groupId ? store.groups.get(*stage_member->groupId) : std::nullopt;
  auto custom_group_volume =
      group ? store.getCustomGroupVolumeByGroupAndDevice(group->_id, *local_device_id) : std::nullopt;

  // Calculate volumes
  double volume = custom_audio_track_volume ? custom_audio_track_volume->volume : audio_track.volume;
  volume *= custom_stage_device_volume ? custom_stage_device_volume->volume : stage_device->volume;
  volume *= custom_stage_member_volume ? custom_stage_member_volume->volume : stage_member->volume;
  if (group) {
    volume *= custom_group_volume ? custom_group_volume->volume : group->volume;
  }
  // Get balance (0 = only me, 1 = only others)
  if (use_balance_) {
    auto balance = calculateBalance(store.getLocalDevice()->balance, audio_track.deviceId == *local_device_id);
    volume *= balance;
  }

  bool muted =
      (custom_stage_member_volume ? custom_stage_member_volume->muted : stage_member->muted) ||
          (custom_stage_device_volume ? custom_stage_device_volume->muted : stage_device->muted) ||
          (custom_audio_track_volume ? custom_audio_track_volume->muted : audio_track.muted);
  if (group) {
    muted = (custom_group_volume ? custom_group_volume->muted : group->muted) || muted;
  }

  std::pair<T, bool> pair = {volume, muted};
  onGainChanged(audio_track._id, pair);
  return pair;
}

template<class T>
std::optional<VolumeInfo<T>>
AudioMixer<T>::getGain(const std::string &audio_track_id) const {
  if (volume_map_.count(audio_track_id)) {
    return volume_map_.at(audio_track_id);
  }
  return std::nullopt;
}
