
#ifndef DS_STORE
#define DS_STORE

#include "DigitalStage/Types.h"
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <set>
#include <optional>

using namespace nlohmann;

namespace DigitalStage {
  namespace Api {

    // Devices
    template <typename TYPE>
    class StoreEntry {
    public:
      std::optional<TYPE> get(const Types::ID_TYPE& id) const
      {
        std::lock_guard<std::recursive_mutex> lock(mutex_store_);
        if(storeEntry_.count(id) > 0) {
          return std::optional<TYPE>(storeEntry_.at(id).template get<const TYPE>());
        }
        return std::nullopt;
      }

      const std::vector<TYPE> getAll() const
      {
        std::lock_guard<std::recursive_mutex> lock(mutex_store_);
        std::vector<TYPE> items = std::vector<TYPE>();
        for(const auto& item : storeEntry_) {
          items.push_back(item.second.template get<TYPE>());
        }
        return items;
      }

      void create(const json& payload)
      {
        std::lock_guard<std::recursive_mutex> lock(mutex_store_);
        const Types::ID_TYPE _id = payload.at("_id").get<std::string>();
        storeEntry_[_id] = payload;
      }

      void update(const json& payload)
      {
        const Types::ID_TYPE& id = payload.at("_id").get<std::string>();
        std::lock_guard<std::recursive_mutex> lock(mutex_store_);
        storeEntry_[id].merge_patch(payload);
      }

      void remove(const Types::ID_TYPE& id)
      {
        std::lock_guard<std::recursive_mutex> lock(mutex_store_);
        storeEntry_.erase(id);
      }

      void removeAll()
      {
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
      mutable std::recursive_mutex ready_mutex_;
      bool isReady_;

      mutable std::recursive_mutex userId_mutex_;
      std::optional<Types::ID_TYPE> userId_;

      mutable std::recursive_mutex stageId_mutex_;
      std::optional<Types::ID_TYPE> stageId_;

      mutable std::recursive_mutex stageMemberId_mutex_;
      std::optional<Types::ID_TYPE> stageMemberId_;

      mutable std::recursive_mutex groupId_mutex_;
      std::optional<Types::ID_TYPE> groupId_;

      mutable std::recursive_mutex local_device_id_mutex_;
      std::optional<Types::ID_TYPE> localDeviceId_;

      mutable std::recursive_mutex stage_device_id_mutex_;
      std::optional<Types::ID_TYPE> stageDeviceId_;

      mutable std::recursive_mutex turn_mutex_;
      std::optional<std::string> turn_username_;
      std::optional<std::string> turn_password_;
      std::vector<std::string> turn_urls_;
    };
  } // namespace Api
} // namespace DigitalStage

#endif // DS_STORE