
#ifndef DS_STORE
#define DS_STORE

#include "Macros.h"
#include "Types.h"
#include <nlohmann/json.hpp>
#include <optional>

using namespace nlohmann;

namespace DigitalStage {
  class Store {
  public:
    // Local device
    std::optional<const DigitalStage::device_t> getLocalDevice();

    void setLocalDeviceId(const std::string& id);
    std::optional<std::string> getLocalDeviceId();

    ADD_STORE_ENTRY(device_t, Device, devices_);
    ADD_STORE_ENTRY(stage_t, Stage, stages_);
    ADD_STORE_ENTRY(user_t, User, user_);
    ADD_STORE_ENTRY(soundcard_t, SoundCard, soundCard_);

    STORE_GETTER(group_t, Group, groups_mutex_, groups_);
    void createGroup(const json payload);
    STORE_UPDATE(group_t, Group, groups_mutex_, groups_);
    void removeGroup(const std::string& id);

  protected:
    std::optional<std::string> localDeviceId_;
    std::recursive_mutex local_device_id_mutex_;

    std::recursive_mutex groups_mutex_;
    std::map<std::string, json> groups_;
    std::map<std::string, std::vector<std::string>> groupIds_by_stages_;
  };
} // namespace DigitalStage

#endif // DS_STORE