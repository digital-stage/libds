#include "ov_client_digitalstage.h"
#include "ds_events.h"
#include "udpsocket.h"
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <pplx/pplxtasks.h>
#include <utility>
#include <vector>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace utility::conversions;
using namespace pplx;
using namespace concurrency::streams;

task_completion_event<void> tce; // used to terminate async PPLX listening task

ov_client_digitalstage_t::ov_client_digitalstage_t(ov_render_base_t& backend,
                                                   std::string api_url)
    : ov_client_base_t(backend), backend_(backend),
      api_url_(std::move(api_url)), ready_(false), quitrequest_(false)
{
  this->sound_card_tools_ = new sound_card_tools_t();
  this->store_ = new ds::ds_store_t();
}

ov_client_digitalstage_t::~ov_client_digitalstage_t()
{
  delete this->sound_card_tools_;
  delete this->store_;
}

void ov_client_digitalstage_t::set_token(const std::string& token)
{
  this->token_ = token;
}

const std::string ov_client_digitalstage_t::get_token()
{
  return this->token_;
}

void ov_client_digitalstage_t::start_service()
{
  if(this->token_.empty())
    throw std::logic_error("Set token first");
  this->servicethread_ = std::thread(&ov_client_digitalstage_t::service, this);
}

void ov_client_digitalstage_t::stop_service()
{
  tce.set(); // task completion event is set closing wss listening task
  this->wsclient_.close(); // wss client is closed
  if(this->servicethread_.joinable())
    this->servicethread_.join(); // thread is joined
}

void ov_client_digitalstage_t::service()
{
  wsclient_.connect(U(this->api_url_)).wait();

  auto receive_task = create_task(tce);

  // handler for incoming d-s messages
  wsclient_.set_message_handler([&](const websocket_incoming_message& ret_msg) {
    // -----------------------------------------------
    // -----    parse incoming message events    -----
    // -----------------------------------------------

    auto ret_str = ret_msg.extract_string().get();

    // hey is our ping, so exlude it
    if(ret_str != "hey") {
      try {
        nlohmann::json j = nlohmann::json::parse(ret_str);

        const std::string& event = j["data"][0];
        const nlohmann::json payload = j["data"][1];

#ifdef DEBUG_EVENTS
        std::cout << "[EVENT] " << event << std::endl;
#endif

        if(event == ds::events::READY) {
          this->ready_ = true;
          if(this->isInsideStage()) {
            std::optional<const ds::device_t> localDevice =
                this->store_->getLocalDevice();
            if(localDevice && localDevice->sendAudio) {
#ifdef DEBUG
              std::cout << "[INFO] Starting all ov related tasks" << std::endl;
#endif
              this->syncLocalStageMember();
              this->syncRemoteStageMembers();
              this->startOv();
            }
          }
          this->store_->dump();
        } else if(event == ds::events::LOCAL_DEVICE_READY) {
          this->store_->setLocalDevice(payload);
          const auto localDevice = this->store_->getLocalDevice();
          // UPDATE SOUND CARDS
          const std::vector<sound_card_t> sound_devices =
              this->sound_card_tools_->get_sound_devices();
          if(!sound_devices.empty()) {
            nlohmann::json deviceUpdate;
            deviceUpdate["_id"] = payload["_id"];
            std::string defaultSoundCardName;
            for(const auto& sound_device : sound_devices) {
              deviceUpdate["soundCardNames"].push_back(sound_device.id);
              if(sound_device.is_default) {
                defaultSoundCardName = sound_device.id;
              }
              nlohmann::json soundCard = {
                  {"name", sound_device.id},
                  {"label", sound_device.name},
                  {"driver", "jack"},
                  {"numInputChannels", sound_device.num_input_channels},
                  {"numOutputChannels", sound_device.num_output_channels},
                  {"sampleRate", sound_device.sample_rate},
                  {"sampleRates", sound_device.sample_rates},
                  {"softwareLatency", sound_device.software_latency},
                  {"isDefault", sound_device.is_default}};
              this->sendAsync("set-sound-card", soundCard.dump());
            }
            if(localDevice->soundCardName.empty() &&
               !defaultSoundCardName.empty()) {
              deviceUpdate["soundCardName"] = defaultSoundCardName;
            }
            this->sendAsync("update-device", deviceUpdate.dump());
          } else {
            std::cerr << "[WARNING] No soundcards available!" << std::endl;
          }
        } else if(event == ds::events::DEVICE_CHANGED) {
          std::optional<const ds::device_t> localDevice =
              this->store_->getLocalDevice();
          if(localDevice && localDevice->_id == payload["_id"]) {
            // The events refers this device
            this->store_->updateLocalDevice(payload);
            if(payload.contains("soundCardName") &&
               payload["soundCardName"] != localDevice->soundCardName) {
              const std::string soundCardName =
                  payload.at("soundCardName").get<std::string>();
              if(soundCardName != localDevice->soundCardName) {
                // Soundcard has been switched
                std::optional<const ds::soundcard_t> soundCard =
                    this->store_->readSoundCardByName(payload["soundCardName"]);
                if(soundCard) {
                  // Use sound card
                  this->configureAudio(*soundCard);
                  // We have to remove all existing tracks and propagate new
                  // tracks
                  std::vector<ds::ov_track_t> existingTracks =
                      this->store_->readOvTracks();
                  for(const auto& track : existingTracks) {
                    this->sendAsync("remove-track", track._id);
                  }
                  for(const auto& channel : soundCard->inputChannels) {
                    this->createTrack(soundCard->_id, channel);
                  }
                } else {
                  std::cerr << "[ERROR] Race condition or logical error: could "
                               "not find "
                               "sound-card propagated by device update"
                            << std::endl;
                }
              }
            }
            if(this->ready_ && this->isInsideStage() &&
               payload.contains("sendAudio")) {
              const bool sendAudio = payload.at("sendAudio").get<bool>();
              if(sendAudio) {
                this->startOv();
              } else {
                this->stopOv();
              }
            }
          }
        } else if(event == ds::events::STAGE_JOINED) {
          const std::string currentStageId =
              payload.at("stageId").get<std::string>();
          this->store_->setCurrentStageId(currentStageId);
          if(payload.contains("users") && payload["users"].is_array()) {
            for(const nlohmann::json& i : payload["users"]) {
              this->store_->createUser(i);
            }
          }
          if(payload.contains("stages") && payload["stages"].is_array()) {
            for(const nlohmann::json& i : payload["stages"]) {
              this->store_->createStage(i);
            }
          }
          if(payload.contains("groups") && payload["groups"].is_array()) {
            for(const nlohmann::json& i : payload["groups"]) {
              this->store_->createGroup(i);
            }
          }
          if(payload.contains("customGroupVolumes") &&
             payload["customGroupVolumes"].is_array()) {
            for(const nlohmann::json& i : payload["customGroupVolumes"]) {
              this->store_->createCustomGroupVolume(i);
            }
          }
          if(payload.contains("customGroupPositions") &&
             payload["customGroupPositions"].is_array()) {
            for(const nlohmann::json& i : payload["customGroupPositions"]) {
              this->store_->createCustomGroupPosition(i);
            }
          }
          if(payload.contains("stageMembers") &&
             payload["stageMembers"].is_array()) {
            for(const nlohmann::json& i : payload["stageMembers"]) {
              this->store_->createStageMember(i);
            }
          }
          if(payload.contains("customStageMemberPositions") &&
             payload["customStageMemberPositions"].is_array()) {
            for(const nlohmann::json& i :
                payload["customStageMemberPositions"]) {
              this->store_->createCustomStageMemberPosition(i);
            }
          }
          if(payload.contains("customStageMemberVolumes") &&
             payload["customStageMemberVolumes"].is_array()) {
            for(const nlohmann::json& i : payload["customStageMemberVolumes"]) {
              this->store_->createCustomStageMemberVolume(i);
            }
          }
          if(payload.contains("ovTracks") && payload["ovTracks"].is_array()) {
            for(const nlohmann::json& i : payload["ovTracks"]) {
              this->store_->createOvTrack(i);
            }
          }
          if(payload.contains("remoteOvTracks") &&
             payload["remoteOvTracks"].is_array()) {
            for(const nlohmann::json& i : payload["remoteOvTracks"]) {
              this->store_->createRemoteOvTrack(i);
            }
          }
          if(payload.contains("customRemoteOvTrackPositions") &&
             payload["customRemoteOvTrackPositions"].is_array()) {
            for(const nlohmann::json& i :
                payload["customRemoteOvTrackPositions"]) {
              this->store_->createCustomRemoteOvTrackPosition(i);
            }
          }
          if(payload.contains("customRemoteOvTrackVolumes") &&
             payload["customRemoteOvTrackVolumes"].is_array()) {
            for(const nlohmann::json& i :
                payload["customRemoteOvTrackVolumes"]) {
              this->store_->createCustomRemoteOvTrackVolume(i);
            }
          }

          if(this->ready_) {
            std::optional<const ds::device_t> localDevice =
                this->store_->getLocalDevice();
            if(localDevice && localDevice->sendAudio) {
              this->syncLocalStageMember();
              this->syncRemoteStageMembers();
              this->startOv();
            }
          } else {
            // Not ready means this is an initial commit, so configure also the
            // stage connection
            std::optional<const ds::stage_t> currentStage =
                this->store_->readStage(currentStageId);

            if(currentStage) {
              if(currentStage->supportsOv) {
                this->configureConnection(currentStage->ovServer);
              } else {
                std::cout
                    << "[WARN] Current stage does not support ov connections"
                    << std::endl;
              }
            } else {
              std::cerr << "[ERROR] Logical error, current stage id "
                        << currentStageId
                        << " defined but stage is not available" << std::endl;
            }
          }
        } else if(event == ds::events::STAGE_LEFT) {
          this->stopOv();
          // Remove active stage related data
          this->store_->removeCustomGroupPositions();
          this->store_->removeCustomGroupVolumes();
          this->store_->removeStageMembers();
          this->store_->removeCustomStageMemberPositions();
          this->store_->removeCustomStageMemberVolumes();
          this->store_->removeRemoteOvTracks();
          this->store_->removeCustomRemoteOvTrackPositions();
          this->store_->removeCustomRemoteOvTrackVolumes();
        } else if(event == ds::events::USER_READY) {
          this->store_->setLocalUser(payload);
        } else if(event == ds::events::STAGE_ADDED) {
          this->store_->createStage(payload);
        } else if(event == ds::events::STAGE_CHANGED) {
          const std::string stageId = payload.at("_id").get<std::string>();
          const std::string currentStageId = this->store_->getCurrentStageId();
          this->store_->updateStage(stageId, payload);
          if(currentStageId == stageId) {
            std::optional<const ds::stage_t> stage =
                this->store_->readStage(currentStageId);
            if(stage) {
              if(stage->supportsOv) {
                this->configureConnection(stage->ovServer);
              } else {
                std::cerr
                    << "[WARN] Current stage does not support ov connections"
                    << std::endl;
              }
            } else {
              std::cerr << "[ERROR] Logical error: stage " << currentStageId
                        << " should existing but is not available" << std::endl;
            }
          }
        } else if(event == ds::events::STAGE_REMOVED) {
          const std::string _id = payload.get<std::string>();
          this->store_->removeStage(_id);
          const std::string currentStageId = this->store_->getCurrentStageId();
          if(currentStageId == _id) {
            this->store_->setCurrentStageId("");
            this->stopOv();
          }
        } else if(event == ds::events::REMOTE_USER_ADDED) {
          this->store_->createUser(payload);
        } else if(event == ds::events::REMOTE_USER_CHANGED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->updateUser(_id, payload);
        } else if(event == ds::events::REMOTE_USER_REMOVED) {
          const std::string _id = payload.get<std::string>();
          this->store_->removeUser(_id);
        } else if(event == ds::events::GROUP_ADDED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->createGroup(payload);
          if(this->isInsideStage()) {
            this->syncGroupPosition(_id);
            this->syncGroupVolume(_id);
          }
        } else if(event == ds::events::GROUP_CHANGED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->updateGroup(_id, payload);
          if(this->isInsideStage()) {
            this->syncGroupPosition(_id);
            this->syncGroupVolume(_id);
          }
        } else if(event == ds::events::GROUP_REMOVED) {
          const std::string _id = payload.get<std::string>();
          this->store_->removeGroup(_id);
          // TODO: Check if triggers are necessary here
        } else if(event == ds::events::CUSTOM_GROUP_POSITION_ADDED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->createCustomGroupPosition(payload);
          if(this->isInsideStage()) {
            this->syncGroupPosition(_id);
          }
        } else if(event == ds::events::CUSTOM_GROUP_POSITION_CHANGED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->updateCustomGroupPosition(_id, payload);
          if(this->isInsideStage()) {
            this->syncGroupPosition(_id);
          }
        } else if(event == ds::events::CUSTOM_GROUP_POSITION_REMOVED) {
          const std::string _id = payload.get<std::string>();
          this->store_->removeCustomGroupPosition(_id);
        } else if(event == ds::events::STAGE_MEMBER_ADDED) {
          this->store_->createStageMember(payload);
          this->syncRemoteStageMembers();
        } else if(event == ds::events::CUSTOM_GROUP_VOLUME_ADDED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->createCustomGroupVolume(payload);
          if(this->isInsideStage()) {
            this->syncGroupPosition(_id);
            this->syncGroupVolume(_id);
          }
        } else if(event == ds::events::CUSTOM_GROUP_VOLUME_CHANGED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->updateCustomGroupVolume(_id, payload);
          if(this->isInsideStage()) {
            this->syncGroupPosition(_id);
            this->syncGroupVolume(_id);
          }
        } else if(event == ds::events::CUSTOM_GROUP_VOLUME_REMOVED) {
          const std::string _id = payload.get<std::string>();
          this->store_->removeCustomGroupVolume(_id);
        } else if(event == ds::events::STAGE_MEMBER_ADDED) {
          this->store_->createStageMember(payload);
          this->syncRemoteStageMembers();
        } else if(event == ds::events::STAGE_MEMBER_CHANGED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->updateStageMember(_id, payload);
          // stage member
          if(this->isInsideStage()) {
            if(payload.contains("volume") || payload.contains("muted")) {
              this->syncStageMemberVolume(_id);
            }
            if(payload.count("x") != 0 || payload.count("z") != 0 ||
               payload.count("y") != 0 || payload.count("rX") != 0 ||
               payload.count("rY") != 0 || payload.count("yZ") != 0) {
              this->syncStageMemberPosition(_id);
            }
          }
        } else if(event == ds::events::STAGE_MEMBER_REMOVED) {
          const std::string _id = payload.get<std::string>();
          this->store_->removeStageMember(_id);
          if(this->isInsideStage()) {
            this->syncRemoteStageMembers();
          }
        } else if(event == ds::events::CUSTOM_STAGE_MEMBER_VOLUME_ADDED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->createCustomStageMemberVolume(payload);
          if(this->isInsideStage()) {
            const std::string stageMemberId =
                payload.at("stageMemberId").get<std::string>();
            this->syncStageMemberVolume(stageMemberId);
          }
        } else if(event == ds::events::CUSTOM_STAGE_MEMBER_VOLUME_CHANGED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->updateCustomStageMemberVolume(_id, payload);
          if(this->isInsideStage()) {
            const auto customStageMember =
                this->store_->readCustomStageMemberVolume(_id);
            if(customStageMember) {
              if(payload.contains("volume") || payload.contains("muted")) {
                this->syncStageMemberVolume(customStageMember->stageMemberId);
              }
            } else {
              std::cerr << "[ERROR] custom stage member not available"
                        << std::endl;
            }
          }
        } else if(event == ds::events::CUSTOM_STAGE_MEMBER_VOLUME_REMOVED) {
          const std::string _id = payload.get<std::string>();
          const auto customStageMember =
              this->store_->readCustomStageMemberVolume(_id);
          this->store_->removeCustomStageMemberVolume(_id);
          if(this->isInsideStage()) {
            if(customStageMember) {
              this->syncStageMemberVolume(customStageMember->stageMemberId);
            } else {
              std::cerr << "[ERROR] custom stage member not available"
                        << std::endl;
            }
          }
        } else if(event == ds::events::CUSTOM_STAGE_MEMBER_POSITION_ADDED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->createCustomStageMemberPosition(payload);
          if(this->isInsideStage()) {
            const std::string stageMemberId =
                payload.at("stageMemberId").get<std::string>();
            this->syncStageMemberPosition(stageMemberId);
          }
        } else if(event == ds::events::CUSTOM_STAGE_MEMBER_POSITION_CHANGED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->updateCustomStageMemberPosition(_id, payload);
          if(this->isInsideStage()) {
            const auto customStageMember =
                this->store_->readCustomStageMemberPosition(_id);
            if(customStageMember) {
              if(payload.contains("x") || payload.contains("z") ||
                 payload.contains("y") || payload.contains("rX") ||
                 payload.contains("rY") || payload.contains("yZ")) {
                this->syncStageMemberPosition(customStageMember->stageMemberId);
              }
            } else {
              std::cerr << "[ERROR] custom stage member not available"
                        << std::endl;
            }
          }
        } else if(event == ds::events::CUSTOM_STAGE_MEMBER_POSITION_REMOVED) {
          const std::string _id = payload.get<std::string>();
          const auto customStageMember =
              this->store_->readCustomStageMemberPosition(_id);
          this->store_->removeCustomStageMemberPosition(_id);
          if(this->isInsideStage()) {
            if(customStageMember) {
              this->syncStageMemberPosition(customStageMember->stageMemberId);
            } else {
              std::cerr << "[ERROR] custom stage member not available"
                        << std::endl;
            }
          }
        } else if(event == ds::events::REMOTE_OV_TRACK_ADDED) {
          this->store_->createRemoteOvTrack(payload);
          if(this->isInsideStage()) {
            this->syncRemoteStageMembers();
          }
        } else if(event == ds::events::REMOTE_OV_TRACK_CHANGED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->updateRemoteOvTrack(_id, payload);
          if(this->isInsideStage()) {
            if(payload.contains("volume") || payload.contains("mute")) {
              this->syncRemoteOvTrackVolume(_id);
            }
            if(payload.contains("x") || payload.contains("y") ||
               payload.contains("x") || payload.contains("rX") ||
               payload.contains("rY") || payload.contains("rZ")) {
              this->syncRemoteOvTrackPosition(_id);
            }
          }
        } else if(event == ds::events::REMOTE_OV_TRACK_REMOVED) {
          const std::string _id = payload.get<std::string>();
          this->store_->removeRemoteOvTrack(_id);
          if(this->isInsideStage()) {
            this->syncRemoteStageMembers();
          }
        } else if(event == ds::events::CUSTOM_REMOTE_OV_TRACK_POSITION_ADDED) {
          this->store_->createCustomRemoteOvTrackPosition(payload);
          if(this->isInsideStage()) {
            const std::string remoteOvTrackId =
                payload.at("remoteOvTrackId").get<std::string>();
            this->syncRemoteOvTrackPosition(remoteOvTrackId);
          }
        } else if(event ==
                  ds::events::CUSTOM_REMOTE_OV_TRACK_POSITION_CHANGED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->updateCustomRemoteOvTrackPosition(_id, payload);
          if(this->isInsideStage()) {
            std::optional<const ds::custom_remote_ov_track_position_t>
                customRemoteOvTrack =
                    this->store_->readCustomRemoteOvTrackPosition(_id);
            if(customRemoteOvTrack) {
              this->syncRemoteOvTrackPosition(
                  customRemoteOvTrack->remoteOvTrackId);
            } else {
              std::cerr << "[ERROR] Could not find required custom remote ov "
                           "track position "
                        << _id << std::endl;
            }
          }
        } else if(event ==
                  ds::events::CUSTOM_REMOTE_OV_TRACK_POSITION_REMOVED) {
          const std::string _id = payload.get<std::string>();
          std::optional<const ds::custom_remote_ov_track_position_t>
              customRemoteOvTrack =
                  this->store_->readCustomRemoteOvTrackPosition(_id);
          this->store_->removeCustomRemoteOvTrackPosition(_id);
          if(this->isInsideStage()) {
            if(customRemoteOvTrack) {
              this->syncRemoteOvTrackPosition(
                  customRemoteOvTrack->remoteOvTrackId);
            } else {
              std::cerr << "[ERROR] Could not find required custom remote ov "
                           "track position "
                        << _id << std::endl;
            }
          }
        } else if(event == ds::events::CUSTOM_REMOTE_OV_TRACK_VOLUME_ADDED) {
          this->store_->createCustomRemoteOvTrackVolume(payload);
          if(this->isInsideStage()) {
            const std::string remoteOvTrackId =
                payload.at("remoteOvTrackId").get<std::string>();
            this->syncRemoteOvTrackVolume(remoteOvTrackId);
          }
        } else if(event == ds::events::CUSTOM_REMOTE_OV_TRACK_VOLUME_CHANGED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->updateCustomRemoteOvTrackPosition(_id, payload);
          if(this->isInsideStage()) {
            std::optional<const ds::custom_remote_ov_track_volume_t>
                customRemoteOvTrackVolume =
                    this->store_->readCustomRemoteOvTrackVolume(_id);
            if(customRemoteOvTrackVolume) {
              this->syncRemoteOvTrackVolume(
                  customRemoteOvTrackVolume->remoteOvTrackId);
            } else {
              std::cerr << "[ERROR] Could not find required custom remote ov "
                           "track volume "
                        << _id << std::endl;
            }
          }
        } else if(event == ds::events::CUSTOM_REMOTE_OV_TRACK_VOLUME_REMOVED) {
          const std::string _id = payload.get<std::string>();
          std::optional<const ds::custom_remote_ov_track_volume_t>
              customRemoteOvTrack =
                  this->store_->readCustomRemoteOvTrackVolume(_id);
          this->store_->removeCustomRemoteOvTrackVolume(_id);
          if(this->isInsideStage()) {
            if(customRemoteOvTrack) {
              this->syncRemoteOvTrackVolume(
                  customRemoteOvTrack->remoteOvTrackId);
            } else {
              std::cerr << "[ERROR] Could not find required custom remote ov "
                           "track volume "
                        << _id << std::endl;
            }
          }
        } else if(event == ds::events::SOUND_CARD_ADDED) {
          this->store_->createSoundCard(payload);
          std::optional<const ds::device_t> localDevice =
              this->store_->getLocalDevice();
          const std::string soundCardName =
              payload.at("name").get<std::string>();
          if(localDevice) {
            if(localDevice->soundCardName.empty()) {
              const nlohmann::json update = {{"_id", localDevice->_id},
                                             {"soundCardName", soundCardName}};
              this->sendAsync("update-device", update.dump());
            } else if(localDevice->soundCardName == soundCardName) {
              const ds::soundcard_t soundCard = payload.get<ds::soundcard_t>();
              this->configureAudio(soundCard);
              for(const unsigned int& channel : soundCard.inputChannels) {
                this->createTrack(soundCard._id, channel);
              }
              if(this->isInsideStage()) {
                this->syncLocalStageMember();
                this->startOv();
              }
            }
          }
        } else if(event == ds::events::SOUND_CARD_CHANGED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->updateSoundCard(_id, payload);
          const auto soundCard = this->store_->readSoundCard(_id);
          const auto localDevice = this->store_->getLocalDevice();
          if(soundCard && localDevice) {
            if(localDevice && localDevice->sendAudio == true &&
               localDevice->soundCardName == soundCard->name) {
              bool isChannelConfigurationDifferent = false;
              std::vector<ds::ov_track_t> existingTracks =
                  this->store_->readOvTracks();
              // Remove obsolete tracks (channel has been removed from sound
              // card for these tracks)
              for(const auto& track : existingTracks) {
                auto it =
                    std::find(soundCard->inputChannels.begin(),
                              soundCard->inputChannels.end(), track.channel);
                if(it == soundCard->inputChannels.end()) {
                  isChannelConfigurationDifferent = true;
                  this->sendAsync("remove-track", track._id);
                }
              }
              // Add missing tracks (channel has been activated on sound card)
              for(const unsigned int& channel : soundCard->inputChannels) {
                auto it = std::find_if(
                    existingTracks.begin(), existingTracks.end(),
                    [channel](const ds::ov_track_t& existingTrack) {
                      return existingTrack.channel == channel;
                    });
                if(it == existingTracks.end()) {
                  isChannelConfigurationDifferent = true;
                  this->createTrack(soundCard->_id, channel);
                }
              }
              if(this->isInsideStage()) {
                if(isChannelConfigurationDifferent) {
                  this->syncLocalStageMember();
                }
              }
            }
          } else {
            std::cerr
                << "[ERROR] Logical error: updated sound card is NOT available";
          }
        } else if(event == ds::events::SOUND_CARD_REMOVED) {
          const std::string _id = payload.get<std::string>();
          std::optional<const ds::soundcard_t> soundCard =
              this->store_->readSoundCard(_id);
          this->store_->removeSoundCard(_id);
          if(this->isInsideStage()) {
            if(soundCard) {
              std::optional<const ds::device_t> localDevice =
                  this->store_->getLocalDevice();
              if(localDevice->soundCardName == soundCard->name) {
                this->stopOv();
              }
            } else {
              std::cerr << "[ERROR] Logical error: stage was not available"
                        << std::endl;
            }
          }
        } else if(event == ds::events::OV_TRACK_ADDED) {
          this->store_->createOvTrack(payload);
        } else if(event == ds::events::OV_TRACK_CHANGED) {
          const std::string _id = payload.at("_id").get<std::string>();
          this->store_->updateOvTrack(_id, payload);
        } else if(event == ds::events::OV_TRACK_REMOVED) {
          const std::string _id = payload.get<std::string>();
          this->store_->removeOvTrack(_id);
        } else {
#ifdef DEBUG_EVENTS
          std::cout << "Not supported: [" << event << "] " << payload.dump()
                    << std::endl;
#endif
        }
      }
      catch(const std::exception& e) {
        std::cerr << "[ERROR] std::exception: " << e.what() << std::endl;
      }
      catch(...) {
        std::cerr << "[ERROR] error parsing" << std::endl;
      }
    }
  });

  // utility::string_t close_reason;
  wsclient_.set_close_handler([](websocket_close_status status,
                                 const utility::string_t& reason,
                                 const std::error_code& code) {
    if(int(status) == 1006) {
      // TODO: Reconnect
      std::cout << "TODO: RECONNECT" << std::endl;
    } else {
      std::cout << " closing reason..." << reason << "\n";
      std::cout << "connection closed, reason: " << reason
                << " close status: " << int(status) << " error code " << code
                << std::endl;
    }
  });

  // Register sound card handler
  // this->soundio->on_devices_change = this->on_sound_devices_change;

  // Get mac address and local ip
  std::string mac(backend_.get_deviceid());

  // Initial call with device
  nlohmann::json deviceJson;
  deviceJson["mac"] = mac; // MAC = device id (!)
  deviceJson["canVideo"] = false;
  deviceJson["canAudio"] = true;
  deviceJson["canOv"] = true;
  deviceJson["sendAudio"] = true;
  deviceJson["sendVideo"] = false;
  deviceJson["receiveAudio"] = true;
  deviceJson["receiveVideo"] = false;

  nlohmann::json identificationJson;
  identificationJson["token"] = this->token_;
  identificationJson["device"] = deviceJson;
  this->sendAsync("token", identificationJson.dump());

  // RECEIVE TILL END
  receive_task.wait();
}

void ov_client_digitalstage_t::on_sound_devices_change()
{
  ucout << "SOUNDCARD CHANGED" << std::endl;
  ucout << "Have now "
        << this->sound_card_tools_->get_input_sound_devices().size()
        << " input soundcards" << std::endl;
}

void ov_client_digitalstage_t::send(const std::string& event,
                                    const std::string& message)
{
  websocket_outgoing_message msg;
  std::string body_str(R"({"type":0,"data":[")" + event + "\"," + message +
                       "]}");
  msg.set_utf8_message(body_str);
#ifdef DEBUG_EVENTS
  std::cout << "[SENDING] " << body_str << std::endl;
#endif
  wsclient_.send(msg).wait();
}

void ov_client_digitalstage_t::sendAsync(const std::string& event,
                                         const std::string& message)
{
  websocket_outgoing_message msg;
  std::string body_str(R"({"type":0,"data":[")" + event + "\"," + message +
                       "]}");
  msg.set_utf8_message(body_str);
#ifdef DEBUG_EVENTS
  std::cout << "[SENDING] " << body_str << std::endl;
#endif
  wsclient_.send(msg);
}

void ov_client_digitalstage_t::createTrack(const std::string& soundCardId,
                                           unsigned int channel)
{
  nlohmann::json trackJson = {{"soundCardId", soundCardId},
                              {"channel", channel}};
  this->sendAsync("add-track", trackJson);
}

void ov_client_digitalstage_t::syncLocalStageMember()
{
  std::lock_guard<std::recursive_mutex>(this->backend_mutex_);
#ifdef DEBUG
  std::cout << "[TRACE] Syncing local stage member" << std::endl;
#endif

  std::optional<const ds::device_t> localDevice =
      this->store_->getLocalDevice();
  std::optional<const ds::user_t> localUser = this->store_->getLocalUser();

  if(!localDevice) {
    std::cerr << "[ERROR] Local device not set yet - server side error?"
              << std::endl;
    return;
  }
  if(!localUser) {
    std::cerr << "[ERROR] Local user not set yet - server side error?"
              << std::endl;
    return;
  }

  std::optional<const ds::stage_member_t> currentStageMember =
      this->store_->getCurrentStageMember();

  if(currentStageMember) {
    // we have to create the tracks, maybe the decorating events has been
    // thrown...
    std::optional<const ds::group_t> group =
        this->store_->readGroup(currentStageMember->groupId);
    std::optional<const ds::custom_stage_member_volume_t>
        customStageMemberVolume =
            this->store_->getCustomStageMemberVolumeByStageMemberId(
                currentStageMember->_id);
    std::optional<const ds::custom_stage_member_position_t>
        customStageMemberPosition =
            this->store_->getCustomStageMemberPositionByStageMemberId(
                currentStageMember->_id);
    std::optional<const ds::custom_group_volume_t> customGroupVolume =
        this->store_->getCustomGroupVolumeByGroupId(group->_id);
    std::optional<const ds::custom_group_position_t> customGroupPosition =
        this->store_->getCustomGroupPositionByGroupId(group->_id);

    const std::vector<ds::remote_ov_track_t> tracks =
        this->store_->getRemoteOvTracksByStageMemberId(currentStageMember->_id);

    std::vector<device_channel_t> deviceChannels;
    // Now for all tracks
    for(const auto& track : tracks) {
#ifdef DEBUG
      std::cout << "[TRACE] Adding local track " << track.channel << std::endl;
#endif
      // Look for custom track
      std::optional<const ds::custom_remote_ov_track_volume_t>
          customTrackVolume =
              this->store_->getCustomOvTrackVolumeByOvTrackId(track._id);
      std::optional<const ds::custom_remote_ov_track_position_t>
          customTrackPosition =
              this->store_->getCustomOvTrackPositionByOvTrackId(track._id);
      double volume =
          customTrackVolume ? customTrackVolume->volume : track.volume;
      pos_t pos;
      if(customTrackPosition) {
        pos = {customTrackPosition->x, customTrackPosition->y,
               customTrackPosition->z};
      } else {
        pos = {track.x, track.y, track.z};
      }
      std::string directivity = customTrackPosition
                                    ? customTrackPosition->directivity
                                    : track.directivity;
      deviceChannels.push_back(
          {track._id, "system:capture_" + std::to_string(track.channel), volume,
           pos, directivity});
    }

    // Calculate stage member volume
    double gain = customStageMemberVolume ? customStageMemberVolume->volume
                                          : currentStageMember->volume;
    gain = customGroupVolume ? (customGroupVolume->volume * gain)
                             : (group->volume * gain);

    bool muted = currentStageMember->muted;
    if(customStageMemberVolume) {
      muted = customStageMemberVolume->muted;
    }
    pos_t position;
    zyx_euler_t orientation;
    if(customStageMemberPosition) {
      position = {
          customStageMemberPosition->x,
          customStageMemberPosition->y,
          customStageMemberPosition->z,
      };
      orientation = {
          customStageMemberPosition->rZ,
          customStageMemberPosition->rY,
          customStageMemberPosition->rX,
      };
    } else {
      position = {
          currentStageMember->x,
          currentStageMember->y,
          currentStageMember->z,
      };
      orientation = {
          currentStageMember->rZ,
          currentStageMember->rY,
          currentStageMember->rX,
      };
    }
    if(customGroupVolume) {
      muted = customGroupVolume->muted || muted;
    }
    if(customGroupPosition) {
      position = {position.x * customGroupPosition->x,
                  position.y * customGroupPosition->y,
                  position.z * customGroupPosition->z};
      orientation = {
          orientation.z * customGroupPosition->rZ,
          orientation.y * customGroupPosition->rY,
          orientation.x * customGroupPosition->rX,
      };
    } else {
      muted = group->muted || muted;
      position = {position.x * group->x, position.y * group->y,
                  position.z * group->z};
      orientation = {
          orientation.z * group->rZ,
          orientation.y * group->rY,
          orientation.x * group->rX,
      };
    }

    if(currentStageMember->ovStageDeviceId == 255) {
      std::cerr << "OV not available" << std::endl;
      return;
    }
    this->backend_.set_thisdev({
        currentStageMember->ovStageDeviceId, localUser->name, deviceChannels,
        position, orientation, gain, muted, localDevice->senderJitter,
        localDevice->receiverJitter,
        true // sendlocal always true?
    });
  } else {
#ifdef DEBUG
    std::cout << "[TRACE] Nothing to do here - not on a stage" << std::endl;
#endif
  }
}

void ov_client_digitalstage_t::syncRemoteStageMembers()
{
  std::lock_guard<std::recursive_mutex>(this->backend_mutex_);
#ifdef DEBUG
  std::cout << "[TRACE] Syncing remote stage members" << std::endl;
#endif
  std::optional<const ds::stage_t> stage = this->store_->getCurrentStage();
  std::optional<const ds::device_t> localDevice =
      this->store_->getLocalDevice();
  if(localDevice && stage) {
    std::map<stage_device_id_t, stage_device_t> stageDeviceMap;

    const std::vector<ds::stage_member_t> stageMembers =
        this->store_->readStageMembersByStage(stage->_id);

    for(const auto& stageMember : stageMembers) {
      std::optional<const ds::user_t> user =
          this->store_->readUser(stageMember.userId);
      std::optional<const ds::group_t> group =
          this->store_->readGroup(stageMember.groupId);
      std::optional<const ds::custom_stage_member_position_t>
          customStageMemberPosition =
              this->store_->getCustomStageMemberPositionByStageMemberId(
                  stageMember._id);
      std::optional<const ds::custom_stage_member_volume_t>
          customStageMemberVolume =
              this->store_->getCustomStageMemberVolumeByStageMemberId(
                  stageMember._id);
      std::optional<const ds::custom_group_position_t> customGroupPosition =
          this->store_->getCustomGroupPositionByGroupId(group->_id);
      std::optional<const ds::custom_group_volume_t> customGroupVolume =
          this->store_->getCustomGroupVolumeByGroupId(group->_id);
      const std::vector<ds::remote_ov_track_t> tracks =
          this->store_->getRemoteOvTracksByStageMemberId(stageMember._id);

      std::vector<device_channel_t> deviceChannels;
      for(const auto& track : tracks) {
        // Look for custom track
        std::optional<const ds::custom_remote_ov_track_position_t>
            customTrackPosition =
                this->store_->getCustomOvTrackPositionByOvTrackId(track._id);
        std::optional<const ds::custom_remote_ov_track_volume_t>
            customTrackVolume =
                this->store_->getCustomOvTrackVolumeByOvTrackId(track._id);

        double volume =
            customTrackVolume ? customTrackVolume->volume : track.volume;
        pos_t pos;
        if(customTrackPosition) {
          pos = {customTrackPosition->x, customTrackPosition->y,
                 customTrackPosition->z};
        }
        std::string directivity = customTrackPosition
                                      ? customTrackPosition->directivity
                                      : track.directivity;
        deviceChannels.push_back({track._id,
                                  "", // TODO: What is sourceport
                                  volume, pos, directivity});
      }

      // Calculate stage member volume
      double gain = customStageMemberVolume ? customStageMemberVolume->volume
                                            : stageMember.volume;
      gain = customGroupVolume ? (customGroupVolume->volume * gain)
                               : (group->volume * gain);
      bool muted = customStageMemberVolume ? customStageMemberVolume->muted
                                           : stageMember.muted;
      pos_t position;
      zyx_euler_t orientation;
      if(customStageMemberPosition) {
        position = {
            customStageMemberPosition->x,
            customStageMemberPosition->y,
            customStageMemberPosition->z,
        };
        orientation = {
            customStageMemberPosition->rZ,
            customStageMemberPosition->rY,
            customStageMemberPosition->rX,
        };
      } else {
        position = {
            stageMember.x,
            stageMember.y,
            stageMember.z,
        };
        orientation = {
            stageMember.rZ,
            stageMember.rY,
            stageMember.rX,
        };
      }
      if(customGroupVolume) {
        muted = customGroupVolume->muted || muted;
      }
      if(customGroupPosition) {
        position = {position.x * customGroupPosition->x,
                    position.y * customGroupPosition->y,
                    position.z * customGroupPosition->z};
        orientation = {
            orientation.z * customGroupPosition->rZ,
            orientation.y * customGroupPosition->rY,
            orientation.x * customGroupPosition->rX,
        };
      } else {
        muted = group->muted || muted;
        position = {position.x * group->x, position.y * group->y,
                    position.z * group->z};
        orientation = {
            orientation.z * group->rZ,
            orientation.y * group->rY,
            orientation.x * group->rX,
        };
      }

      stage_device_t stageDevice = {stageMember.ovStageDeviceId,
                                    user ? user->name : stageMember._id,
                                    deviceChannels,
                                    position,
                                    orientation,
                                    gain,
                                    muted,
                                    localDevice->senderJitter,
                                    localDevice->receiverJitter,
                                    stageMember.sendlocal};
      stageDeviceMap[stageDevice.id] = stageDevice;
    }
    this->backend_.set_stage(stageDeviceMap);
  } else {
#ifdef DEBUG
    std::cout << "[TRACE] Nothing to do here - not on a stage" << std::endl;
#endif
  }
}

void ov_client_digitalstage_t::syncStageMemberPosition(
    const std::string& stageMemberId)
{
#ifdef DEBUG
  std::cout << "[TRACE] Syncing position of stage member " << stageMemberId
            << std::endl;
#endif
  std::lock_guard<std::recursive_mutex>(this->backend_mutex_);
  std::optional<const ds::stage_member_t> stageMember =
      this->store_->readStageMember(stageMemberId);

  if(stageMember) {
    std::optional<const ds::group_t> group =
        this->store_->readGroup(stageMember->groupId);
    std::optional<const ds::custom_stage_member_position_t>
        customStageMemberPosition =
            this->store_->getCustomStageMemberPositionByStageMemberId(
                stageMember->_id);
    std::optional<const ds::custom_group_position_t> customGroupPosition =
        this->store_->getCustomGroupPositionByGroupId(group->_id);
    pos_t position;
    zyx_euler_t orientation;
    if(customStageMemberPosition) {
      position = {
          customStageMemberPosition->x,
          customStageMemberPosition->y,
          customStageMemberPosition->z,
      };
      orientation = {
          customStageMemberPosition->rZ,
          customStageMemberPosition->rY,
          customStageMemberPosition->rX,
      };
    } else {
      position = {
          stageMember->x,
          stageMember->y,
          stageMember->z,
      };
      orientation = {
          stageMember->rZ,
          stageMember->rY,
          stageMember->rX,
      };
    }
    if(customGroupPosition) {
      position = {position.x * customGroupPosition->x,
                  position.y * customGroupPosition->y,
                  position.z * customGroupPosition->z};
      orientation = {
          orientation.z * customGroupPosition->rZ,
          orientation.y * customGroupPosition->rY,
          orientation.x * customGroupPosition->rX,
      };
    } else {
      position = {position.x * group->x, position.y * group->y,
                  position.z * group->z};
      orientation = {
          orientation.z * group->rZ,
          orientation.y * group->rY,
          orientation.x * group->rX,
      };
    }
    this->backend_.set_stage_device_position(stageMember->ovStageDeviceId,
                                             position, orientation);

    this->store_->dump();
  }
}

void ov_client_digitalstage_t::syncStageMemberVolume(
    const std::string& stageMemberId)
{
#ifdef DEBUG
  std::cout << "[TRACE] Syncing volume of stage member " << stageMemberId
            << std::endl;
#endif
  std::lock_guard<std::recursive_mutex>(this->backend_mutex_);
  std::optional<const ds::stage_member_t> stageMember =
      this->store_->readStageMember(stageMemberId);

  if(stageMember) {
    std::optional<const ds::group_t> group =
        this->store_->readGroup(stageMember->groupId);
    std::optional<const ds::custom_stage_member_volume_t>
        customStageMemberVolume =
            this->store_->getCustomStageMemberVolumeByStageMemberId(
                stageMember->_id);
    std::optional<const ds::custom_group_volume_t> customGroupVolume =
        this->store_->getCustomGroupVolumeByGroupId(group->_id);
    double gain = customStageMemberVolume ? customStageMemberVolume->volume
                                          : stageMember->volume;
    gain = customGroupVolume ? (customGroupVolume->volume * gain)
                             : (group->volume * gain);
    // TODO: mute is MISSING!!!
    // bool muted = (customStageMember ? customStageMember->muted :
    // stageMember->muted)
    //    || (customGroup ? customGroup->muted : group->muted);
    this->backend_.set_stage_device_gain(stageMember->ovStageDeviceId, gain);

    this->store_->dump();
  }
}

void ov_client_digitalstage_t::startOv()
{
  std::lock_guard<std::recursive_mutex>(this->backend_mutex_);
  if(!this->backend_.is_session_active()) {
#ifdef DEBUG
    std::cout << "[INFO] Starting OV Transmission, since it is not running yet"
              << std::endl;
#endif
    if(this->backend_.is_audio_active()) {
      this->backend_.start_session();
    } else {
      std::cout << "[WARN] Did not start, since no audio backend is available"
                << std::endl;
    }
  }
}

void ov_client_digitalstage_t::stopOv()
{
  std::lock_guard<std::recursive_mutex>(this->backend_mutex_);
#ifdef DEBUG
  std::cout << "[TRACE] Stopping OV Transmission (even if it is not running)"
            << std::endl;
#endif
  this->backend_.clear_stage();
  if(this->backend_.is_audio_active()) {
    this->backend_.stop_audiobackend();
  }
}

void ov_client_digitalstage_t::configureAudio(const ds::soundcard_t soundCard)
{
  std::lock_guard<std::recursive_mutex>(this->backend_mutex_);
#ifdef DEBUG
  std::cout << "[INFO] Configure Audio using sound card " << soundCard.name
            << std::endl;
  std::cout << "SOUND CARD ID:" << soundCard.name << std::endl;
#endif
  this->backend_.configure_audio_backend(
      {soundCard.driver, soundCard.name, soundCard.sampleRate,
       soundCard.periodSize, soundCard.numPeriods});
  if(!this->backend_.is_audio_active()) {
    this->backend_.start_audiobackend();
  }
}

void ov_client_digitalstage_t::configureConnection(
    const ds::stage_ov_server_t server)
{
  std::lock_guard<std::recursive_mutex>(this->backend_mutex_);
#ifdef DEBUG
  std::cout << "[INFO] Configure connection using server " << server.ipv4 << ":"
            << server.port << std::endl;
#endif
  this->backend_.set_relay_server(server.ipv4, server.port, server.pin);
}

void ov_client_digitalstage_t::syncGroupVolume(const std::string& groupId)
{
#ifdef DEBUG
  std::cout << "[TRACE] Syncing volume of group " << groupId << std::endl;
#endif
  const std::vector<ds::stage_member_t> stageMembers =
      this->store_->readStageMembersByGroup(groupId);
  for(const auto& stageMember : stageMembers) {
    this->syncStageMemberVolume(stageMember._id);
  }
}

void ov_client_digitalstage_t::syncGroupPosition(const std::string& groupId)
{
#ifdef DEBUG
  std::cout << "[TRACE] Syncing position of group " << groupId << std::endl;
#endif
  const std::vector<ds::stage_member_t> stageMembers =
      this->store_->readStageMembersByGroup(groupId);
  for(const auto& stageMember : stageMembers) {
    this->syncStageMemberPosition(stageMember._id);
  }
}

void ov_client_digitalstage_t::syncRemoteOvTrackVolume(
    const std::string& remoteOvTrackId)
{
#ifdef DEBUG
  std::cout << "[TRACE] Syncing volume of remote track " << remoteOvTrackId
            << std::endl;
#endif
  std::lock_guard<std::recursive_mutex>(this->backend_mutex_);
  std::optional<const ds::remote_ov_track_t> remoteOvTrack =
      this->store_->readRemoteOvTrack(remoteOvTrackId);
  if(remoteOvTrack) {
    std::optional<const ds::stage_member_t> stageMember =
        this->store_->readStageMember(remoteOvTrack->stageMemberId);

    if(stageMember) {
      std::optional<const ds::custom_remote_ov_track_volume_t>
          customRemoteOvTrackVolume =
              this->store_->getCustomOvTrackVolumeByOvTrackId(remoteOvTrackId);
      const double gain = customRemoteOvTrackVolume
                              ? customRemoteOvTrackVolume->volume
                              : remoteOvTrack->volume;
      this->backend_.set_stage_device_channel_gain(stageMember->ovStageDeviceId,
                                                   remoteOvTrackId, gain);
    } else {
      std::cerr << "[ERROR] Could not find stage member "
                << remoteOvTrack->stageMemberId << std::endl;
    }
  } else {
    std::cerr << "[ERROR] Could not sync remote ov track " << remoteOvTrackId
              << ", since it is not available" << std::endl;
  }
}

void ov_client_digitalstage_t::syncRemoteOvTrackPosition(
    const std::string& remoteOvTrackId)
{
  std::lock_guard<std::recursive_mutex>(this->backend_mutex_);
#ifdef DEBUG
  std::cout << "[TRACE] Syncing position of remote track " << remoteOvTrackId
            << std::endl;
#endif
  std::optional<const ds::remote_ov_track_t> remoteOvTrack =
      this->store_->readRemoteOvTrack(remoteOvTrackId);
  if(remoteOvTrack) {
    std::optional<const ds::stage_member_t> stageMember =
        this->store_->readStageMember(remoteOvTrack->stageMemberId);
    if(stageMember) {
      std::optional<const ds::custom_remote_ov_track_position_t>
          customRemoteOvTrackPosition =
              this->store_->getCustomOvTrackPositionByOvTrackId(
                  remoteOvTrackId);
      if(customRemoteOvTrackPosition) {
        this->backend_.set_stage_device_channel_position(
            stageMember->ovStageDeviceId, remoteOvTrackId,
            {customRemoteOvTrackPosition->x, customRemoteOvTrackPosition->y,
             customRemoteOvTrackPosition->z},
            {customRemoteOvTrackPosition->rZ, customRemoteOvTrackPosition->rY,
             customRemoteOvTrackPosition->rX});
      } else {
        this->backend_.set_stage_device_channel_position(
            stageMember->ovStageDeviceId, remoteOvTrackId,
            {remoteOvTrack->x, remoteOvTrack->y, remoteOvTrack->z},
            {remoteOvTrack->rZ, remoteOvTrack->rY, remoteOvTrack->rX});
      }
    } else {
      std::cerr << "[ERROR] Could not find stage member "
                << remoteOvTrack->stageMemberId << std::endl;
    }
  } else {
    std::cerr << "[ERROR] Could not sync remote ov track " << remoteOvTrackId
              << ", since it is not available" << std::endl;
  }
}
bool ov_client_digitalstage_t::isInsideStage()
{
  std::lock_guard<std::recursive_mutex>(this->backend_mutex_);
  return !this->store_->getCurrentStageId().empty();
}
