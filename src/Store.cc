
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
  if(groupIds_by_stages_.count(stageId) == 0) {
    groupIds_by_stages_[stageId] = std::set<std::string>();
  }
  groupIds_by_stages_[stageId].insert(_id);
}

void DigitalStage::Store::removeGroup(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  const std::string stageId =
      this->groups_.at(id)["stageId"].get<std::string>();
  this->groups_.erase(id);
  groupIds_by_stages_[stageId].erase(id);
}

std::vector<const DigitalStage::group_t>
DigitalStage::Store::getGroupsByStage(const std::string& stageId) const
{
  std::lock_guard<std::recursive_mutex>(this->groups_mutex_);
  auto groups = std::vector<const DigitalStage::group_t>();
  if(this->groupIds_by_stages_.count(stageId) > 0) {
    auto groupIds = this->groupIds_by_stages_.at(stageId);
    for(const auto& groupId : groupIds) {
      auto group = getGroup(groupId);
      if(group) {
        groups.push_back(group.value());
      }
    }
  }
  return groups;
}

void DigitalStage::Store::createStageMember(const nlohmann::json payload)
{
  std::lock_guard<std::recursive_mutex>(this->stageMembers_mutex_);
  const std::string _id = payload.at("_id").get<std::string>();
  stageMembers_[_id] = payload;
  const std::string stageId = payload.at("stageId").get<std::string>();
  const std::string groupId = payload.at("groupId").get<std::string>();
  if(stageMemberIds_by_stages_.count(stageId) == 0) {
    stageMemberIds_by_stages_[stageId] = std::set<std::string>();
  }
  stageMemberIds_by_stages_[stageId].insert(_id);
  if(stageMemberIds_by_groups_.count(groupId) == 0) {
    stageMemberIds_by_groups_[stageId] = std::set<std::string>();
  }
  stageMemberIds_by_groups_[groupId].insert(_id);
}

void DigitalStage::Store::removeStageMember(const std::string& id)
{
  std::lock_guard<std::recursive_mutex>(this->stageMembers_mutex_);
  const std::string stageId =
      this->stageMembers_.at(id)["stageId"].get<std::string>();
  const std::string groupId =
      this->stageMembers_.at(id)["groupId"].get<std::string>();
  this->stageMembers_.erase(id);
  stageMemberIds_by_stages_[stageId].erase(id);
  stageMemberIds_by_groups_[groupId].erase(id);
}

std::vector<const DigitalStage::stage_member_t>
DigitalStage::Store::getStageMembersByStage(const std::string& stageId) const
{
  std::lock_guard<std::recursive_mutex>(this->stageMembers_mutex_);
  auto vector = std::vector<const DigitalStage::stage_member_t>();
  if(this->stageMemberIds_by_stages_.count(stageId) > 0) {
    auto stageMemberIds = this->stageMemberIds_by_stages_.at(stageId);
    for(const auto& stageMemberId : stageMemberIds) {
      auto stageMember = getStageMember(stageMemberId);
      if(stageMember) {
        vector.push_back(stageMember.value());
      }
    }
  }
  return vector;
}

std::vector<const DigitalStage::stage_member_t>
DigitalStage::Store::getStageMembersByGroup(const std::string& groupId) const
{
  std::lock_guard<std::recursive_mutex>(this->stageMembers_mutex_);
  auto vector = std::vector<const DigitalStage::stage_member_t>();
  if(this->stageMemberIds_by_groups_.count(groupId) > 0) {
    auto stageMemberIds = this->stageMemberIds_by_groups_.at(groupId);
    for(const auto& stageMemberId : stageMemberIds) {
      auto stageMember = getStageMember(stageMemberId);
      if(stageMember) {
        vector.push_back(stageMember.value());
      }
    }
  }
  return vector;
}