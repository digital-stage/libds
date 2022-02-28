
#ifndef DS_STORE
#define DS_STORE

#include "DigitalStage/Types.h"
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <set>
#include <optional>
#include <atomic>

#include <spdlog/spdlog.h>

namespace DigitalStage {
  namespace Api {

    using namespace nlohmann;

    // This is a thread-safe container for our types, stored in json format in a map
    template <typename TYPE>
    class StoreEntry {
    public:
      std::optional<TYPE> get(const Types::ID_TYPE& id) const noexcept
      {
        std::lock_guard<std::mutex> lock(mutex_store_);
        if(storeEntry_.count(id) > 0) {
            try {
                return std::optional<TYPE>(storeEntry_.at(id).template get<const TYPE>());
            }
            catch (Types::ParseException const &e) {
                spdlog::error("Error in type conversion from json, returning nullopt: {}", e.what());
            }
        }
        return std::nullopt;
      }

      std::vector<TYPE> getAll() const
      {
        std::lock_guard<std::mutex> lock(mutex_store_);
        std::vector<TYPE> items = std::vector<TYPE>();
        for (const auto& item : storeEntry_) {
            try {
                items.push_back(item.second.template get<TYPE>());
            }
            catch (Types::ParseException const& e)
            {
                spdlog::error("Error in type conversion from json, not part of returned list: {}", e.what());
            }
        }
        return items;
      }

      void create(const json& payload)
      {
        std::lock_guard<std::mutex> lock(mutex_store_);
          if (validate(payload)) {
              const Types::ID_TYPE _id = payload.at("_id").get<Types::ID_TYPE>();
              storeEntry_[_id] = payload;
          }
      }

      void update(const json& payload)
      {
        const Types::ID_TYPE& id = payload.at("_id").get<Types::ID_TYPE>();
        std::lock_guard<std::mutex> lock(mutex_store_);
        auto saved = storeEntry_[id];
        storeEntry_[id].merge_patch(payload);
        if (!validate(storeEntry_[id])) {
            spdlog::error("Differential update destroyed JSON validity, patch not applied!");
            storeEntry_[id] = saved;
        }
      }

      void remove(const Types::ID_TYPE& id)
      {
        std::lock_guard<std::mutex> lock(mutex_store_);
        if (storeEntry_.find(id) != storeEntry_.end()) {
            storeEntry_.erase(id);
        }
        else {
            spdlog::error("Cannot remove object, id not found in storeEntry: {}", id);
        }
      }

      void removeAll()
      {
        std::lock_guard<std::mutex> lock(mutex_store_);
        storeEntry_.clear();
      }

        bool validate(const json& payload) const
      {
            try {
                payload.get<TYPE>();
                return true;
            }
            catch (Types::ParseException const& e) {
                spdlog::warn("Error in type validation from json: {}", e.what());
            }
            return false;
      }

    private:
      mutable std::mutex mutex_store_;
      std::map<std::string, json> storeEntry_;
    };

    template <class T>
    class LockedOptionalValue {
    public:
        std::optional<T> get() const {
            std::lock_guard<std::mutex> lock(mutex_);
            if (id_.has_value()) {
                return id_;
            }
            return std::nullopt;
        }

        bool has_value() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return id_.has_value();
        }

        void set(std::optional<T> new_value) {
            std::lock_guard<std::mutex> lock(mutex_);
            id_ = new_value;
        }

    private:
        mutable std::mutex mutex_;
        std::optional<T> id_;
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
      std::optional<DigitalStage::Types::Device> getLocalDevice() const;
      void setLocalDeviceId(const Types::ID_TYPE& id);
      /**
       * Returns the ID of this (local) device
       * @return
       */
      std::optional<Types::ID_TYPE> getLocalDeviceId() const;

      // Devices
      StoreEntry<DigitalStage::Types::Device> devices;

      // Local user
      void setUserId(const Types::ID_TYPE& id);
      /**
       * Returns the ID of the current user
       * @return ID of the current user
       */
      std::optional<Types::ID_TYPE> getUserId() const;
      void setStageMemberId(const Types::ID_TYPE& id);
      /**
       * Returns the ID of the current stage member
       * This will be null if the user is currently not inside any stage
       * @return ID of the current stage member or null
       */
      [[maybe_unused]] std::optional<Types::ID_TYPE> getStageMemberId() const;

      // Users
      StoreEntry<DigitalStage::Types::User> users;

      /*
       * Stage management
       */
      void setStageId(const Types::ID_TYPE& id);
      void resetStageId();
      void resetStageMemberId();
      /**
       * Returns the ID of the current stage.
       * This will return null if the user is currently not inside any stage.
       * Use this variable to obtain, if the user is currently inside a stage.
       * @return ID of the current stage or null
       */
      std::optional<Types::ID_TYPE> getStageId() const;

      /**
       * Return the current stage, if available
       * @return current stage
       */
      std::optional<DigitalStage::Types::Stage> getStage() const;

      void setGroupId(std::optional<Types::ID_TYPE> id);
      void resetGroupId();
      /**
       * Returns the ID of the current group.
       * This will return null if the user is currently not inside any stage.
       * @return ID of the current group or null
       */
      [[maybe_unused]] std::optional<Types::ID_TYPE> getGroupId() const;

      void setStageDeviceId(const Types::ID_TYPE& id);
      void resetStageDeviceId();
      /**
       * Returns the ID of this (local) stage device.
       * A stage device is an abstraction of this device for all stage members.
       * This will return null if the user is currently not inside any stage.
       * @return ID of this stage device or null
       */
      std::optional<Types::ID_TYPE> getStageDeviceId() const;
      /**
       * Returns the this (local) stage device.
       * This will return null if the user is currently not inside any stage.
       * @return this stage device or null
       */
      std::optional<DigitalStage::Types::StageDevice> getStageDevice() const;

      /**
       * Sets the given TURN/STUN servers
       * @param turn_servers
       */
      void setTurnServers(std::vector<std::string> turn_servers);

      /**
       * Returns a list of url of TURN/STUN servers.
       * Prefix them with turn: or stun: by your own needs.
       * @return list of TURN/STUN server urls
       */
      std::vector<std::string> getTurnServers() const;

      /**
       * Sets the given TURN/STUN username
       * @param username
       */
      void setTurnUsername(const std::string& username);

      /**
       * Returns the username to authenticate on the TURN servers
       * @return username
       */
      std::optional<std::string> getTurnUsername() const;

      /**
       * Sets the given TURN/STUN username
       * @param username
       */
      void setTurnPassword(const Types::ID_TYPE& username);

      /**
       * Returns the password to authenticate on the TURN servers.
       * @return password
       */
      std::optional<std::string> getTurnPassword() const;

      // Stages
      StoreEntry<DigitalStage::Types::Stage> stages;

      // Groups
      StoreEntry<DigitalStage::Types::Group> groups;
      std::vector<DigitalStage::Types::Group> getGroupsByStage(const Types::ID_TYPE& stageId) const;

      // Stage members
      StoreEntry<DigitalStage::Types::StageMember> stageMembers;
      [[maybe_unused]] std::vector<DigitalStage::Types::StageMember>
      getStageMembersByStage(const Types::ID_TYPE& stageId) const;
      std::vector<DigitalStage::Types::StageMember>
      getStageMembersByGroup(const Types::ID_TYPE& audio_trackId) const;

      // Stage devices
      StoreEntry<DigitalStage::Types::StageDevice> stageDevices;
      std::vector<DigitalStage::Types::StageDevice>
      getStageDevicesByStageMember(const Types::ID_TYPE& stageMemberId) const;

      // Video tracks
      StoreEntry<DigitalStage::Types::VideoTrack> videoTracks;
      std::vector<DigitalStage::Types::VideoTrack>
      getVideoTracksByStageDevice(const Types::ID_TYPE& stageDeviceId) const;

      // Audio tracks
      using AudioTracks = std::vector<DigitalStage::Types::AudioTrack>;
      StoreEntry<DigitalStage::Types::AudioTrack> audioTracks;
      std::optional<DigitalStage::Types::AudioTrack> getAudioTrackByUuid(const Types::ID_TYPE & uuid) const;
      AudioTracks getAudioTracksByStageDevice(const Types::ID_TYPE& stageDeviceId) const;
      AudioTracks getAudioTracksByStageMember(const Types::ID_TYPE& stageMemberId) const;
      AudioTracks getAudioTracksByGroup(const Types::ID_TYPE& groupId) const;
      AudioTracks getLocalAudioTracks() const;

      // Custom Groups
      StoreEntry<DigitalStage::Types::CustomGroup> customGroups;
      std::optional<DigitalStage::Types::CustomGroup> getCustomGroupByGroupAndTargetGroup(const Types::ID_TYPE& groupId, const Types::ID_TYPE& targetGroupId) const;

      // Sound cards
      StoreEntry<DigitalStage::Types::SoundCard> soundCards;
      std::optional<DigitalStage::Types::SoundCard>
      getSoundCardByDeviceAndDriverAndTypeAndLabel(const Types::ID_TYPE& deviceId,
                                                   const std::string& audioDriver,
                                                   const std::string& type,
                                                   const std::string& label) const;
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
      std::atomic<bool> isReady_;

      LockedOptionalValue<Types::ID_TYPE> userId_;
      LockedOptionalValue<Types::ID_TYPE> stageId_;
      LockedOptionalValue<Types::ID_TYPE> stageMemberId_;
      LockedOptionalValue<Types::ID_TYPE> groupId_;
      LockedOptionalValue<Types::ID_TYPE> localDeviceId_;
      LockedOptionalValue<Types::ID_TYPE> stageDeviceId_;

      LockedOptionalValue<std::string> turn_username_;
      LockedOptionalValue<std::string> turn_password_;
      mutable std::mutex turn_url_mutex_;
      std::vector<std::string> turn_urls_;
    };
  } // namespace Api
} // namespace DigitalStage

#endif // DS_STORE
