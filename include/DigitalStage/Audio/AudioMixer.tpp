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
  client_->stageMemberChanged.connect([this](const std::string &stage_member_id, const nlohmann::json &update,
                                             const DigitalStage::Api::Store *store) {
    if (update.contains("volume") || update.contains("muted") || update.contains("groupId")) {
      // Find and update all related audio tracks
      for (const auto &audio_track: store->audioTracks.getAll()) {
        if (audio_track.stageMemberId == stage_member_id) {
          volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
        }
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
  client_->customGroupAdded.connect([this](const CustomGroup &custom_group,
                                           const DigitalStage::Api::Store *store) {
    auto group_id = store->getGroupId();
    if (group_id && custom_group.targetGroupId == group_id) {
      for (const auto &stage_member: store->getStageMembersByGroup(custom_group.groupId)) {
        for (const auto &audio_track: store->audioTracks.getAll()) {
          if (audio_track.stageMemberId == stage_member._id) {
            volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
          }
        }
      }
    }
  }, token_);
  client_->customGroupChanged.connect([this](const std::string &custom_group_id,
                                             const nlohmann::json &update,
                                             const DigitalStage::Api::Store *store) {
    if (update.contains("volume") || update.contains("muted")) {
      auto custom_group = store->customGroups.get(custom_group_id);
      assert(custom_group);
      auto group_id = store->getGroupId();
      if (group_id && custom_group->targetGroupId == group_id) {
        // Find and update all related audio tracks
        for (const auto &stage_member: store->getStageMembersByGroup(custom_group->groupId)) {
          for (const auto &audio_track: store->audioTracks.getAll()) {
            if (audio_track.stageMemberId == stage_member._id) {
              volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
            }
          }
        }
      }
    }
  }, token_);
  client_->customGroupRemoved.connect([this](const CustomGroup &custom_group,
                                             const DigitalStage::Api::Store *store) {
    auto group_id = store->getGroupId();
    if (group_id && custom_group.targetGroupId == group_id) {
      for (const auto &stage_member: store->getStageMembersByGroup(custom_group.groupId)) {
        for (const auto &audio_track: store->audioTracks.getAll()) {
          if (audio_track.stageMemberId == stage_member._id) {
            volume_map_[audio_track._id] = calculateVolume(audio_track, *store);
          }
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
  auto group_id = store.getGroupId();

  // Get related stage member
  auto stage_member = store.stageMembers.get(audio_track.stageMemberId);
  assert(stage_member);

  // Get related group
  auto group = stage_member->groupId ? store.groups.get(*stage_member->groupId) : std::nullopt;
  auto custom_group =
      (group_id && stage_member->groupId) ? store.getCustomGroupByGroupAndTargetGroup(*stage_member->groupId, *group_id)
                                          : std::nullopt;

  // Calculate volumes
  std::cout << "Calculating volume: " << std::endl;
  std::cout << "(audio-track) " << std::to_string(audio_track.volume) << std::endl;
  std::cout << "(stage-device) " << std::to_string(stage_member->volume) << std::endl;
  if (custom_group) {
    std::cout << "(custom-group) " << std::to_string(custom_group->volume) << std::endl;
  } else {
    std::cout << "(group) " << std::to_string(group->volume) << std::endl;
  }

  double volume = audio_track.volume * stage_member->volume;
  if (custom_group) {
    volume *= custom_group->volume;
  } else if (group) {
    volume *= group->volume;
  }
  // Get balance (0 = only me, 1 = only others)
  if (use_balance_) {
    auto balance = calculateBalance(store.getLocalDevice()->balance, audio_track.deviceId == *local_device_id);
    std::cout << "(balance) " << balance;
    volume *= balance;
  }
  std::cout << "Resulting volume:  " << volume << std::endl;

  bool muted = stage_member->muted || audio_track.muted;
  if (custom_group) {
    muted = custom_group->muted || muted;
  } else if (group) {
    muted = group->muted || muted;
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
