
#include "Store.h"

std::optional<const DigitalStage::device_t>
DigitalStage::Store::getLocalDevice()
{
  std::lock_guard<std::recursive_mutex>(this->local_device_id_mutex_);
  if(localDeviceId_) {
    return this->getDevice(localDeviceId_.value());
  }
  return std::nullopt;
}
std::optional<std::string> DigitalStage::Store::getLocalDeviceId()
{
  std::lock_guard<std::recursive_mutex>(this->local_device_id_mutex_);
  return localDeviceId_;
}

void DigitalStage::Store::setLocalDeviceId(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->local_device_id_mutex_);
  localDeviceId_ = id;
}

void DigitalStage::Store::createGroup(const nlohmann::json payload)
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  groups_[_id] = payload;
  const std::string stageId = payload.at("stageId").get<std::string>();
  if(groupIds_by_stages_.count(stageId) > 0) {
    groupIds_by_stages_[stageId] = std::vector<std::string>();
  }
  groupIds_by_stages_[stageId].push_back(_id);
}

void DigitalStage::Store::removeGroup(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  const std::string stageId =
      this->groups_.at(id)["stageId"].get<std::string>();
  this->groups_.erase(id);
  groupIds_by_stages_[stageId].push_back(id);
}