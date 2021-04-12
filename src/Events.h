#ifndef DS_EVENTS
#define DS_EVENTS

#include <string>

namespace DigitalStage {
  namespace Events {
    extern const std::string READY;
    extern const std::string LOCAL_DEVICE_READY;
    extern const std::string DEVICE_CHANGED;
    extern const std::string STAGE_JOINED;
    extern const std::string STAGE_LEFT;
    extern const std::string USER_READY;
    extern const std::string STAGE_ADDED;
    extern const std::string STAGE_CHANGED;
    extern const std::string STAGE_REMOVED;
    extern const std::string REMOTE_USER_ADDED;
    extern const std::string REMOTE_USER_CHANGED;
    extern const std::string REMOTE_USER_REMOVED;
    extern const std::string GROUP_ADDED;
    extern const std::string GROUP_CHANGED;
    extern const std::string GROUP_REMOVED;
    extern const std::string STAGE_MEMBER_ADDED;
    extern const std::string STAGE_MEMBER_CHANGED;
    extern const std::string STAGE_MEMBER_REMOVED;
    extern const std::string CUSTOM_GROUP_VOLUME_ADDED;
    extern const std::string CUSTOM_GROUP_VOLUME_CHANGED;
    extern const std::string CUSTOM_GROUP_VOLUME_REMOVED;
    extern const std::string CUSTOM_GROUP_POSITION_ADDED;
    extern const std::string CUSTOM_GROUP_POSITION_CHANGED;
    extern const std::string CUSTOM_GROUP_POSITION_REMOVED;
    extern const std::string CUSTOM_STAGE_MEMBER_POSITION_ADDED;
    extern const std::string CUSTOM_STAGE_MEMBER_POSITION_CHANGED;
    extern const std::string CUSTOM_STAGE_MEMBER_POSITION_REMOVED;
    extern const std::string CUSTOM_STAGE_MEMBER_VOLUME_ADDED;
    extern const std::string CUSTOM_STAGE_MEMBER_VOLUME_CHANGED;
    extern const std::string CUSTOM_STAGE_MEMBER_VOLUME_REMOVED;
    extern const std::string REMOTE_OV_TRACK_ADDED;
    extern const std::string REMOTE_OV_TRACK_CHANGED;
    extern const std::string REMOTE_OV_TRACK_REMOVED;
    extern const std::string CUSTOM_REMOTE_OV_TRACK_VOLUME_ADDED;
    extern const std::string CUSTOM_REMOTE_OV_TRACK_VOLUME_CHANGED;
    extern const std::string CUSTOM_REMOTE_OV_TRACK_VOLUME_REMOVED;
    extern const std::string CUSTOM_REMOTE_OV_TRACK_POSITION_ADDED;
    extern const std::string CUSTOM_REMOTE_OV_TRACK_POSITION_CHANGED;
    extern const std::string CUSTOM_REMOTE_OV_TRACK_POSITION_REMOVED;
    extern const std::string SOUND_CARD_ADDED;
    extern const std::string SOUND_CARD_CHANGED;
    extern const std::string SOUND_CARD_REMOVED;
  } // namespace events
} // namespace ds

#endif // DS_EVENTS