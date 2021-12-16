//
// Created by Tobias Hegemann on 04.11.21.
//
#pragma once

#include "DigitalStage/Api/Client.h"
#include "DigitalStage/Api/Store.h"
#include "DigitalStage/Types.h"
#include <string>
#include <unordered_map>
#include <optional>
#include <utility>
#include <sigslot/signal.hpp>

namespace DigitalStage {
namespace Audio {
template<class T> using VolumeInfo = std::pair<T /* volume */, bool /* muted */>;

template<class T>
class AudioMixer {
 public:
  explicit AudioMixer(std::shared_ptr<DigitalStage::Api::Client> client, bool use_balance = false);

  void applyGain(const std::string &audio_track_id, T *data, std::size_t frame_count);
  T applyGain(const std::string &audio_track_id, T data);
  std::optional<VolumeInfo<T>> getGain(const std::string &audio_track_id) const;

  sigslot::signal<std::string, std::pair<T, bool>> onGainChanged;
 private:
  void attachHandlers();
  std::pair<T, bool> calculateVolume(const DigitalStage::Types::AudioTrack &audio_track,
                                     const DigitalStage::Api::Store &store);
  static double calculateBalance(double balance, bool is_local);

  std::unordered_map<std::string, std::pair<T, bool>> volume_map_;
  std::shared_ptr<DigitalStage::Api::Client> client_;
  std::shared_ptr<DigitalStage::Api::Client::Token> token_;
  bool use_balance_;
};

}
}
#include "AudioMixer.tpp"