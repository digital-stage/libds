#include "DigitalStage/Api/Events.h"

using namespace DigitalStage::Api;

const std::string SendEvents::CHANGE_DEVICE = "change-device";
const std::string SendEvents::SET_SOUND_CARD = "set-sound-card";
const std::string SendEvents::CHANGE_SOUND_CARD = "change-sound-card";
const std::string SendEvents::CREATE_AUDIO_TRACK =
    "create-audio-track";
const std::string SendEvents::CHANGE_AUDIO_TRACK =
    "create-audio-track";
const std::string SendEvents::REMOVE_AUDIO_TRACK =
    "remove-audio-track";
const std::string SendEvents::SEND_P2P_RESTART =
    "send-p2p-restart";
const std::string SendEvents::SEND_P2P_OFFER =
    "send-p2p-offer";
const std::string SendEvents::SEND_P2P_ANSWER =
    "send-p2p-answer";
const std::string SendEvents::SEND_ICE_CANDIDATE =
    "send-ice-candidate";

const std::string SendEvents::CREATE_STAGE =
    "create-stage";
const std::string SendEvents::CHANGE_STAGE =
    "change-stage";
const std::string SendEvents::REMOVE_STAGE =
    "remove-stage";

const std::string SendEvents::CREATE_GROUP =
    "create-group";
const std::string SendEvents::CHANGE_GROUP =
    "change-group";
const std::string SendEvents::REMOVE_GROUP =
    "remove-group";

const std::string SendEvents::JOIN_STAGE =
    "join-stage";
const std::string SendEvents::LEAVE_STAGE =
    "leave-stage";
const std::string SendEvents::LEAVE_STAGE_FOR_GOOD =
    "leave-stage-for-good";

const std::string RetrieveEvents::READY = "ok";
const std::string RetrieveEvents::LOCAL_DEVICE_READY = "ldr";
const std::string RetrieveEvents::DEVICE_ADDED = "d-a";
const std::string RetrieveEvents::DEVICE_CHANGED = "d-c";
const std::string RetrieveEvents::DEVICE_REMOVED = "d-r";
const std::string RetrieveEvents::STAGE_JOINED = "s-j";
const std::string RetrieveEvents::STAGE_LEFT = "s-l";
const std::string RetrieveEvents::USER_READY = "u-r";
const std::string RetrieveEvents::STAGE_ADDED = "s-a";
const std::string RetrieveEvents::STAGE_CHANGED = "s-c";
const std::string RetrieveEvents::STAGE_REMOVED = "s-r";
const std::string RetrieveEvents::REMOTE_USER_ADDED = "r-u-a";
const std::string RetrieveEvents::REMOTE_USER_CHANGED = "r-u-c";
const std::string RetrieveEvents::REMOTE_USER_REMOVED = "r-u-r";
const std::string RetrieveEvents::GROUP_ADDED = "g-a";
const std::string RetrieveEvents::GROUP_CHANGED = "g-c";
const std::string RetrieveEvents::GROUP_REMOVED = "g-r";
const std::string RetrieveEvents::STAGE_MEMBER_ADDED = "sm-a";
const std::string RetrieveEvents::STAGE_MEMBER_CHANGED = "sm-c";
const std::string RetrieveEvents::STAGE_MEMBER_REMOVED = "sm-r";
const std::string RetrieveEvents::STAGE_DEVICE_ADDED = "sd-a";
const std::string RetrieveEvents::STAGE_DEVICE_CHANGED = "sd-c";
const std::string RetrieveEvents::STAGE_DEVICE_REMOVED = "sd-r";
const std::string RetrieveEvents::CUSTOM_GROUP_POSITION_ADDED = "c-g-p-a";
const std::string RetrieveEvents::CUSTOM_GROUP_POSITION_CHANGED = "c-g-p-c";
const std::string RetrieveEvents::CUSTOM_GROUP_POSITION_REMOVED = "c-g-p-r";
const std::string RetrieveEvents::CUSTOM_GROUP_VOLUME_ADDED = "c-g-v-a";
const std::string RetrieveEvents::CUSTOM_GROUP_VOLUME_CHANGED = "c-g-v-c";
const std::string RetrieveEvents::CUSTOM_GROUP_VOLUME_REMOVED = "c-g-v-r";
const std::string RetrieveEvents::CUSTOM_STAGE_MEMBER_VOLUME_ADDED = "c-sm-v-a";
const std::string RetrieveEvents::CUSTOM_STAGE_MEMBER_VOLUME_CHANGED =
    "c-sm-v-c";
const std::string RetrieveEvents::CUSTOM_STAGE_MEMBER_VOLUME_REMOVED =
    "c-sm-v-r";
const std::string RetrieveEvents::CUSTOM_STAGE_MEMBER_POSITION_ADDED =
    "c-sm-p-a";
const std::string RetrieveEvents::CUSTOM_STAGE_MEMBER_POSITION_CHANGED =
    "c-sm-p-c";
const std::string RetrieveEvents::CUSTOM_STAGE_MEMBER_POSITION_REMOVED =
    "c-sm-p-r";
const std::string RetrieveEvents::CUSTOM_STAGE_DEVICE_VOLUME_ADDED = "c-sd-v-a";
const std::string RetrieveEvents::CUSTOM_STAGE_DEVICE_VOLUME_CHANGED =
    "c-sd-v-c";
const std::string RetrieveEvents::CUSTOM_STAGE_DEVICE_VOLUME_REMOVED =
    "c-sd-v-r";
const std::string RetrieveEvents::CUSTOM_STAGE_DEVICE_POSITION_ADDED =
    "c-sd-p-a";
const std::string RetrieveEvents::CUSTOM_STAGE_DEVICE_POSITION_CHANGED =
    "c-sd-p-c";
const std::string RetrieveEvents::CUSTOM_STAGE_DEVICE_POSITION_REMOVED =
    "c-sd-p-r";
const std::string RetrieveEvents::VIDEO_TRACK_ADDED = "v-a";
const std::string RetrieveEvents::VIDEO_TRACK_CHANGED = "v-c";
const std::string RetrieveEvents::VIDEO_TRACK_REMOVED = "v-r";
const std::string RetrieveEvents::AUDIO_TRACK_ADDED = "a-a";
const std::string RetrieveEvents::AUDIO_TRACK_CHANGED = "a-c";
const std::string RetrieveEvents::AUDIO_TRACK_REMOVED = "a-r";
const std::string RetrieveEvents::CUSTOM_AUDIO_TRACK_VOLUME_ADDED =
    "c-a-v-a";
const std::string RetrieveEvents::CUSTOM_AUDIO_TRACK_VOLUME_CHANGED =
    "c-a-v-a";
const std::string RetrieveEvents::CUSTOM_AUDIO_TRACK_VOLUME_REMOVED =
    "c-a-v-r";
const std::string RetrieveEvents::CUSTOM_AUDIO_TRACK_POSITION_ADDED =
    "c-a-p-a";
const std::string RetrieveEvents::CUSTOM_AUDIO_TRACK_POSITION_CHANGED =
    "c-a-p-c";
const std::string RetrieveEvents::CUSTOM_AUDIO_TRACK_POSITION_REMOVED =
    "c-a-p-r";
const std::string RetrieveEvents::SOUND_CARD_ADDED = "sc-a";
const std::string RetrieveEvents::SOUND_CARD_CHANGED = "sc-c";
const std::string RetrieveEvents::SOUND_CARD_REMOVED = "sc-r";

const std::string RetrieveEvents::P2P_RESTART = "p2p-r";
const std::string RetrieveEvents::P2P_OFFER_SENT = "p2p-o";
const std::string RetrieveEvents::P2P_ANSWER_SENT = "p2p-a";
const std::string RetrieveEvents::ICE_CANDIDATE_SENT = "ice";
const std::string RetrieveEvents::TURN_SERVERS_CHANGED = "t";