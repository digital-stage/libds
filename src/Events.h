#ifndef DS_EVENTS
#define DS_EVENTS

#include "Types.h"
#include "eventpp/utilities/eventmaker.h"
#include <nlohmann/json.hpp>
#include <string>

namespace DigitalStage {
  enum class EventType {
    READY,
    LOCAL_DEVICE_READY,
    DEVICE_ADDED,
    DEVICE_CHANGED,
    DEVICE_REMOVED,
    STAGE_JOINED,
    STAGE_LEFT,
    USER_READY,
    STAGE_ADDED,
    STAGE_CHANGED,
    STAGE_REMOVED,
    REMOTE_USER_ADDED,
    REMOTE_USER_CHANGED,
    REMOTE_USER_REMOVED,
    GROUP_ADDED,
    GROUP_CHANGED,
    GROUP_REMOVED,
    STAGE_MEMBER_ADDED,
    STAGE_MEMBER_CHANGED,
    STAGE_MEMBER_REMOVED,
    CUSTOM_GROUP_VOLUME_ADDED,
    CUSTOM_GROUP_VOLUME_CHANGED,
    CUSTOM_GROUP_VOLUME_REMOVED,
    CUSTOM_GROUP_POSITION_ADDED,
    CUSTOM_GROUP_POSITION_CHANGED,
    CUSTOM_GROUP_POSITION_REMOVED,
    CUSTOM_STAGE_MEMBER_POSITION_ADDED,
    CUSTOM_STAGE_MEMBER_POSITION_CHANGED,
    CUSTOM_STAGE_MEMBER_POSITION_REMOVED,
    CUSTOM_STAGE_MEMBER_VOLUME_ADDED,
    CUSTOM_STAGE_MEMBER_VOLUME_CHANGED,
    CUSTOM_STAGE_MEMBER_VOLUME_REMOVED,
    LOCAL_VIDEO_TRACK_ADDED,
    LOCAL_VIDEO_TRACK_CHANGED,
    LOCAL_VIDEO_TRACK_REMOVED,
    LOCAL_AUDIO_TRACK_ADDED,
    LOCAL_AUDIO_TRACK_CHANGED,
    LOCAL_AUDIO_TRACK_REMOVED,
    REMOTE_VIDEO_TRACK_ADDED,
    REMOTE_VIDEO_TRACK_CHANGED,
    REMOTE_VIDEO_TRACK_REMOVED,
    REMOTE_AUDIO_TRACK_ADDED,
    REMOTE_AUDIO_TRACK_CHANGED,
    REMOTE_AUDIO_TRACK_REMOVED,
    CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_ADDED,
    CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_CHANGED,
    CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_REMOVED,
    CUSTOM_REMOTE_AUDIO_TRACK_POSITION_ADDED,
    CUSTOM_REMOTE_AUDIO_TRACK_POSITION_CHANGED,
    CUSTOM_REMOTE_AUDIO_TRACK_POSITION_REMOVED,
    SOUND_CARD_ADDED,
    SOUND_CARD_CHANGED,
    SOUND_CARD_REMOVED
  };

  class Event {
  public:
    explicit Event(const EventType type_) : type(type_) {}
    virtual ~Event() {}

    EventType getType() const { return type; }

  private:
    EventType type;
  };

  EVENTPP_MAKE_EMPTY_EVENT(EventReady, Event, EventType::READY);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow-field-in-constructor"
  EVENTPP_MAKE_EVENT(EventLocalDeviceReady, Event,
                     EventType::LOCAL_DEVICE_READY, (device_t, getDevice));

  EVENTPP_MAKE_EVENT(EventDeviceAdded, Event, EventType::DEVICE_ADDED,
                     (device_t, getDevice));
  EVENTPP_MAKE_EVENT(EventDeviceChanged, Event, EventType::DEVICE_CHANGED,
                     (std::string, getId), (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventDeviceRemoved, Event, EventType::DEVICE_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventStageAdded, Event, EventType::STAGE_ADDED,
                     (stage_t, getStage));
  EVENTPP_MAKE_EVENT(EventStageChanged, Event, EventType::STAGE_CHANGED,
                     (std::string, getId), (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventStageRemoved, Event, EventType::STAGE_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventGroupAdded, Event, EventType::GROUP_ADDED,
                     (group_t, getGroup));
  EVENTPP_MAKE_EVENT(EventGroupChanged, Event, EventType::GROUP_CHANGED,
                     (std::string, getId), (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventGroupRemoved, Event, EventType::GROUP_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventCustomGroupPositionAdded, Event,
                     EventType::CUSTOM_GROUP_POSITION_ADDED,
                     (custom_group_position_t, getCustomGroupPosition));
  EVENTPP_MAKE_EVENT(EventCustomGroupPositionChanged, Event,
                     EventType::CUSTOM_GROUP_POSITION_CHANGED,
                     (std::string, getId), (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventCustomGroupPositionRemoved, Event,
                     EventType::CUSTOM_GROUP_POSITION_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventCustomGroupVolumeAdded, Event,
                     EventType::CUSTOM_GROUP_VOLUME_ADDED,
                     (custom_group_volume_t, getCustomGroupVolume));
  EVENTPP_MAKE_EVENT(EventCustomGroupVolumeChanged, Event,
                     EventType::CUSTOM_GROUP_VOLUME_CHANGED,
                     (std::string, getId), (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventCustomGroupVolumeRemoved, Event,
                     EventType::CUSTOM_GROUP_VOLUME_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventStageMemberAdded, Event,
                     EventType::STAGE_MEMBER_ADDED,
                     (stage_member_t, getStageMember));
  EVENTPP_MAKE_EVENT(EventStageMemberChanged, Event,
                     EventType::STAGE_MEMBER_CHANGED, (std::string, getId),
                     (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventStageMemberRemoved, Event,
                     EventType::STAGE_MEMBER_REMOVED, (std::string, getId));

  EVENTPP_MAKE_EVENT(EventCustomStageMemberPositionAdded, Event,
                     EventType::CUSTOM_STAGE_MEMBER_POSITION_ADDED,
                     (custom_stage_member_position_t,
                      getCustomStageMemberPosition));
  EVENTPP_MAKE_EVENT(EventCustomStageMemberPositionChanged, Event,
                     EventType::CUSTOM_STAGE_MEMBER_POSITION_CHANGED,
                     (std::string, getId), (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventCustomStageMemberPositionRemoved, Event,
                     EventType::CUSTOM_STAGE_MEMBER_POSITION_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventCustomStageMemberVolumeAdded, Event,
                     EventType::CUSTOM_STAGE_MEMBER_VOLUME_ADDED,
                     (custom_stage_member_volume_t,
                      getCustomStageMemberVolume));
  EVENTPP_MAKE_EVENT(EventCustomStageMemberVolumeChanged, Event,
                     EventType::CUSTOM_STAGE_MEMBER_VOLUME_CHANGED,
                     (std::string, getId), (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventCustomStageMemberVolumeRemoved, Event,
                     EventType::CUSTOM_STAGE_MEMBER_VOLUME_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventLocalVideoTrackAdded, Event,
                     EventType::LOCAL_VIDEO_TRACK_ADDED,
                     (local_video_track_t, getLocalVideoTrack));
  EVENTPP_MAKE_EVENT(EventLocalVideoTrackChanged, Event,
                     EventType::LOCAL_VIDEO_TRACK_CHANGED, (std::string, getId),
                     (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventLocalVideoTrackRemoved, Event,
                     EventType::LOCAL_VIDEO_TRACK_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventLocalAudioTrackAdded, Event,
                     EventType::LOCAL_AUDIO_TRACK_ADDED,
                     (local_audio_track_t, getLocalAudioTrack));
  EVENTPP_MAKE_EVENT(EventLocalAudioTrackChanged, Event,
                     EventType::LOCAL_AUDIO_TRACK_CHANGED, (std::string, getId),
                     (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventLocalAudioTrackRemoved, Event,
                     EventType::LOCAL_AUDIO_TRACK_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventRemoteVideoTrackAdded, Event,
                     EventType::REMOTE_VIDEO_TRACK_ADDED,
                     (remote_video_track_t, getRemoteVideoTrack));
  EVENTPP_MAKE_EVENT(EventRemoteVideoTrackChanged, Event,
                     EventType::REMOTE_VIDEO_TRACK_CHANGED,
                     (std::string, getId), (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventRemoteVideoTrackRemoved, Event,
                     EventType::REMOTE_VIDEO_TRACK_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventRemoteAudioTrackAdded, Event,
                     EventType::REMOTE_AUDIO_TRACK_ADDED,
                     (remote_audio_track_t, getRemoteAudioTrack));
  EVENTPP_MAKE_EVENT(EventRemoteAudioTrackChanged, Event,
                     EventType::REMOTE_AUDIO_TRACK_CHANGED,
                     (std::string, getId), (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventRemoteAudioTrackRemoved, Event,
                     EventType::REMOTE_AUDIO_TRACK_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventCustomRemoteAudioTrackPositionAdded, Event,
                     EventType::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_ADDED,
                     (custom_remote_audio_track_position_t,
                      getCustomRemoteAudioTrackPosition));
  EVENTPP_MAKE_EVENT(EventCustomRemoteAudioTrackPositionChanged, Event,
                     EventType::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_CHANGED,
                     (std::string, getId), (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventCustomRemoteAudioTrackPositionRemoved, Event,
                     EventType::CUSTOM_REMOTE_AUDIO_TRACK_POSITION_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventCustomRemoteAudioTrackVolumeAdded, Event,
                     EventType::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_ADDED,
                     (custom_remote_audio_track_volume_t,
                      getCustomRemoteAudioTrackVolume));
  EVENTPP_MAKE_EVENT(EventCustomRemoteAudioTrackVolumeChanged, Event,
                     EventType::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_CHANGED,
                     (std::string, getId), (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventCustomRemoteAudioTrackVolumeRemoved, Event,
                     EventType::CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventSoundCardAdded, Event, EventType::SOUND_CARD_ADDED,
                     (soundcard_t, getSoundCard));
  EVENTPP_MAKE_EVENT(EventSoundCardChanged, Event,
                     EventType::SOUND_CARD_CHANGED, (std::string, getId),
                     (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventSoundCardRemoved, Event,
                     EventType::SOUND_CARD_REMOVED, (std::string, getId));

  EVENTPP_MAKE_EVENT(EventLocalUserReady, Event, EventType::USER_READY,
                     (std::string, getId));
  EVENTPP_MAKE_EVENT(EventUserAdded, Event, EventType::REMOTE_USER_ADDED,
                     (user_t, getUser));
  EVENTPP_MAKE_EVENT(EventUserChanged, Event, EventType::REMOTE_USER_CHANGED,
                     (std::string, getId), (nlohmann::json, getUpdate));
  EVENTPP_MAKE_EVENT(EventUserRemoved, Event, EventType::REMOTE_USER_REMOVED,
                     (std::string, getId));

  EVENTPP_MAKE_EVENT(EventStageJoined, Event, EventType::STAGE_JOINED,
                     (std::string, getStageId), (std::string, getGroupId));
#pragma clang diagnostic pop

  EVENTPP_MAKE_EMPTY_EVENT(EventStageLeft, Event, EventType::STAGE_LEFT);

  namespace WSEvents {
    extern const std::string READY;
    extern const std::string LOCAL_DEVICE_READY;
    extern const std::string DEVICE_ADDED;
    extern const std::string DEVICE_CHANGED;
    extern const std::string DEVICE_REMOVED;
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
    extern const std::string LOCAL_VIDEO_TRACK_ADDED;
    extern const std::string LOCAL_VIDEO_TRACK_CHANGED;
    extern const std::string LOCAL_VIDEO_TRACK_REMOVED;
    extern const std::string LOCAL_AUDIO_TRACK_ADDED;
    extern const std::string LOCAL_AUDIO_TRACK_CHANGED;
    extern const std::string LOCAL_AUDIO_TRACK_REMOVED;
    extern const std::string REMOTE_VIDEO_TRACK_ADDED;
    extern const std::string REMOTE_VIDEO_TRACK_CHANGED;
    extern const std::string REMOTE_VIDEO_TRACK_REMOVED;
    extern const std::string REMOTE_AUDIO_TRACK_ADDED;
    extern const std::string REMOTE_AUDIO_TRACK_CHANGED;
    extern const std::string REMOTE_AUDIO_TRACK_REMOVED;
    extern const std::string CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_ADDED;
    extern const std::string CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_CHANGED;
    extern const std::string CUSTOM_REMOTE_AUDIO_TRACK_VOLUME_REMOVED;
    extern const std::string CUSTOM_REMOTE_AUDIO_TRACK_POSITION_ADDED;
    extern const std::string CUSTOM_REMOTE_AUDIO_TRACK_POSITION_CHANGED;
    extern const std::string CUSTOM_REMOTE_AUDIO_TRACK_POSITION_REMOVED;
    extern const std::string SOUND_CARD_ADDED;
    extern const std::string SOUND_CARD_CHANGED;
    extern const std::string SOUND_CARD_REMOVED;
  } // namespace WSEvents
} // namespace DigitalStage

#endif // DS_EVENTS