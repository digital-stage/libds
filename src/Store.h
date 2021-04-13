
#ifndef DS_STORE
#define DS_STORE

#include "Macros.h"
#include "Types.h"
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <set>

using namespace nlohmann;

namespace DigitalStage {
  class Store {
  public:
    // Local device
    std::optional<const DigitalStage::device_t> getLocalDevice();

    void setLocalDeviceId(const std::string& id);
    std::optional<std::string> getLocalDeviceId();

    STORE_GET(group_t, Group, groups_mutex_, groups_);
    STORE_GET_ALL(group_t, Groups, groups_mutex_, groups_);
    std::vector<const group_t>
    getGroupsByStage(const std::string& stageId) const;
    void createGroup(const json payload);
    STORE_UPDATE(group_t, Group, groups_mutex_, groups_);
    void removeGroup(const std::string& id);

    STORE_GET(stage_member_t, StageMember, stageMembers_mutex_, stageMembers_);
    STORE_GET_ALL(stage_member_t, StageMembers, stageMembers_mutex_,
                  stageMembers_);
    std::vector<const stage_member_t>
    getStageMembersByStage(const std::string& stageId) const;
    std::vector<const stage_member_t>
    getStageMembersByGroup(const std::string& groupId) const;
    void createStageMember(const json payload);
    STORE_UPDATE(stage_member_t, StageMember, stageMembers_mutex_,
                 stageMembers_);
    void removeStageMember(const std::string& id);

    ADD_STORE_ENTRY(device_t, Device, devices_);
    ADD_STORE_ENTRY(stage_t, Stage, stages_);
    ADD_STORE_ENTRY(user_t, User, user_);
    ADD_STORE_ENTRY(soundcard_t, SoundCard, soundCard_);

  protected:
    mutable std::recursive_mutex local_device_id_mutex_;
    std::optional<std::string> localDeviceId_;

    mutable std::recursive_mutex groups_mutex_;
    std::map<std::string, json> groups_;
    std::map<std::string, std::set<std::string>> groupIds_by_stages_;

    mutable std::recursive_mutex stageMembers_mutex_;
    std::map<std::string, json> stageMembers_;
    std::map<std::string, std::set<std::string>> stageMemberIds_by_stages_;
    std::map<std::string, std::set<std::string>> stageMemberIds_by_groups_;
  };
} // namespace DigitalStage

#endif // DS_STORE