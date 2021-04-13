#include "Events.h"

const std::string DigitalStage::WSEvents::READY = "ok";
const std::string DigitalStage::WSEvents::LOCAL_DEVICE_READY = "ldr";
const std::string DigitalStage::WSEvents::DEVICE_ADDED = "d-a";
const std::string DigitalStage::WSEvents::DEVICE_CHANGED = "d-c";
const std::string DigitalStage::WSEvents::DEVICE_REMOVED = "d-r";
const std::string DigitalStage::WSEvents::STAGE_JOINED = "s-j";
const std::string DigitalStage::WSEvents::STAGE_LEFT = "s-l";
const std::string DigitalStage::WSEvents::USER_READY = "user-ready";
const std::string DigitalStage::WSEvents::STAGE_ADDED = "s-a";
const std::string DigitalStage::WSEvents::STAGE_CHANGED = "s-c";
const std::string DigitalStage::WSEvents::STAGE_REMOVED = "s-r";
const std::string DigitalStage::WSEvents::REMOTE_USER_ADDED = "r-u-a";
const std::string DigitalStage::WSEvents::REMOTE_USER_CHANGED = "r-u-c";
const std::string DigitalStage::WSEvents::REMOTE_USER_REMOVED = "r-u-r";
const std::string DigitalStage::WSEvents::GROUP_ADDED = "g-a";
const std::string DigitalStage::WSEvents::GROUP_CHANGED = "g-c";
const std::string DigitalStage::WSEvents::GROUP_REMOVED = "g-r";
const std::string DigitalStage::WSEvents::STAGE_MEMBER_ADDED = "sm-a";
const std::string DigitalStage::WSEvents::STAGE_MEMBER_CHANGED = "sm-c";
const std::string DigitalStage::WSEvents::STAGE_MEMBER_REMOVED = "sm-r";
const std::string DigitalStage::WSEvents::CUSTOM_GROUP_POSITION_ADDED =
    "c-g-p-a";
const std::string DigitalStage::WSEvents::CUSTOM_GROUP_POSITION_CHANGED =
    "c-g-p-c";
const std::string DigitalStage::WSEvents::CUSTOM_GROUP_POSITION_REMOVED =
    "c-g-p-r";
const std::string DigitalStage::WSEvents::CUSTOM_GROUP_VOLUME_ADDED = "c-g-v-a";
const std::string DigitalStage::WSEvents::CUSTOM_GROUP_VOLUME_CHANGED =
    "c-g-v-c";
const std::string DigitalStage::WSEvents::CUSTOM_GROUP_VOLUME_REMOVED =
    "c-g-v-r";
const std::string DigitalStage::WSEvents::CUSTOM_STAGE_MEMBER_VOLUME_ADDED =
    "c-sm-v-a";
const std::string DigitalStage::WSEvents::CUSTOM_STAGE_MEMBER_VOLUME_CHANGED =
    "c-sm-v-c";
const std::string DigitalStage::WSEvents::CUSTOM_STAGE_MEMBER_VOLUME_REMOVED =
    "c-sm-v-r";
const std::string DigitalStage::WSEvents::CUSTOM_STAGE_MEMBER_POSITION_ADDED =
    "c-sm-p-a";
const std::string DigitalStage::WSEvents::CUSTOM_STAGE_MEMBER_POSITION_CHANGED =
    "c-sm-p-c";
const std::string DigitalStage::WSEvents::CUSTOM_STAGE_MEMBER_POSITION_REMOVED =
    "c-sm-p-r";
const std::string DigitalStage::WSEvents::REMOTE_VIDEO_TRACK_ADDED = "v-a";
const std::string DigitalStage::WSEvents::REMOTE_VIDEO_TRACK_CHANGED = "v-c";
const std::string DigitalStage::WSEvents::REMOTE_VIDEO_TRACK_REMOVED = "v-r";
const std::string DigitalStage::WSEvents::REMOTE_AUDIO_TRACK_ADDED = "a-a";
const std::string DigitalStage::WSEvents::REMOTE_AUDIO_TRACK_CHANGED = "a-c";
const std::string DigitalStage::WSEvents::REMOTE_AUDIO_TRACK_REMOVED = "a-r";
const std::string
    DigitalStage::WSEvents::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_ADDED = "c-a-v-a";
const std::string
    DigitalStage::WSEvents::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_CHANGED =
        "c-a-v-a";
const std::string
    DigitalStage::WSEvents::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_REMOVED =
        "c-a-v-r";
const std::string
    DigitalStage::WSEvents::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_ADDED =
        "c-a-p-a";
const std::string
    DigitalStage::WSEvents::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_CHANGED =
        "c-a-p-c";
const std::string
    DigitalStage::WSEvents::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_REMOVED =
        "c-a-p-r";
const std::string DigitalStage::WSEvents::SOUND_CARD_ADDED = "sound-card-added";
const std::string DigitalStage::WSEvents::SOUND_CARD_CHANGED =
    "sound-card-changed";
const std::string DigitalStage::WSEvents::SOUND_CARD_REMOVED =
    "sound-card-removed";