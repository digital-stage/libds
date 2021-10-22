#ifndef DS_EVENTS
#define DS_EVENTS

#include <DigitalStage/Types.h>
#include <nlohmann/json.hpp>
#include <string>

using namespace DigitalStage::Types;

namespace DigitalStage::Api {
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
      STAGE_DEVICE_ADDED,
      STAGE_DEVICE_CHANGED,
      STAGE_DEVICE_REMOVED,
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
      CUSTOM_STAGE_DEVICE_POSITION_ADDED,
      CUSTOM_STAGE_DEVICE_POSITION_CHANGED,
      CUSTOM_STAGE_DEVICE_POSITION_REMOVED,
      CUSTOM_STAGE_DEVICE_VOLUME_ADDED,
      CUSTOM_STAGE_DEVICE_VOLUME_CHANGED,
      CUSTOM_STAGE_DEVICE_VOLUME_REMOVED,
      VIDEO_TRACK_ADDED,
      VIDEO_TRACK_CHANGED,
      VIDEO_TRACK_REMOVED,
      AUDIO_TRACK_ADDED,
      AUDIO_TRACK_CHANGED,
      AUDIO_TRACK_REMOVED,
      CUSTOM_AUDIO_TRACK_VOLUME_ADDED,
      CUSTOM_AUDIO_TRACK_VOLUME_CHANGED,
      CUSTOM_AUDIO_TRACK_VOLUME_REMOVED,
      CUSTOM_AUDIO_TRACK_POSITION_ADDED,
      CUSTOM_AUDIO_TRACK_POSITION_CHANGED,
      CUSTOM_AUDIO_TRACK_POSITION_REMOVED,
      SOUND_CARD_ADDED,
      SOUND_CARD_CHANGED,
      SOUND_CARD_REMOVED,
      P2P_RESTART,
      P2P_OFFER_SENT,
      P2P_ANSWER_SENT,
      ICE_CANDIDATE_SENT,
    };

    namespace SendEvents {
      [[maybe_unused]] extern const std::string CHANGE_DEVICE;
      extern const std::string SET_SOUND_CARD;
      extern const std::string CHANGE_SOUND_CARD;
      extern const std::string CREATE_AUDIO_TRACK;
      [[maybe_unused]] extern const std::string CHANGE_AUDIO_TRACK;
      extern const std::string REMOVE_AUDIO_TRACK;
      extern const std::string SEND_P2P_RESTART;
      extern const std::string SEND_P2P_OFFER;
      extern const std::string SEND_P2P_ANSWER;
      extern const std::string SEND_ICE_CANDIDATE;
    } // namespace SendEvents

    namespace RetrieveEvents {
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
      extern const std::string STAGE_DEVICE_ADDED;
      extern const std::string STAGE_DEVICE_CHANGED;
      extern const std::string STAGE_DEVICE_REMOVED;
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
      extern const std::string CUSTOM_STAGE_DEVICE_POSITION_ADDED;
      extern const std::string CUSTOM_STAGE_DEVICE_POSITION_CHANGED;
      extern const std::string CUSTOM_STAGE_DEVICE_POSITION_REMOVED;
      extern const std::string CUSTOM_STAGE_DEVICE_VOLUME_ADDED;
      extern const std::string CUSTOM_STAGE_DEVICE_VOLUME_CHANGED;
      extern const std::string CUSTOM_STAGE_DEVICE_VOLUME_REMOVED;
      extern const std::string VIDEO_TRACK_ADDED;
      extern const std::string VIDEO_TRACK_CHANGED;
      extern const std::string VIDEO_TRACK_REMOVED;
      extern const std::string AUDIO_TRACK_ADDED;
      extern const std::string AUDIO_TRACK_CHANGED;
      extern const std::string AUDIO_TRACK_REMOVED;
      extern const std::string CUSTOM_AUDIO_TRACK_VOLUME_ADDED;
      extern const std::string CUSTOM_AUDIO_TRACK_VOLUME_CHANGED;
      extern const std::string CUSTOM_AUDIO_TRACK_VOLUME_REMOVED;
      extern const std::string CUSTOM_AUDIO_TRACK_POSITION_ADDED;
      extern const std::string CUSTOM_AUDIO_TRACK_POSITION_CHANGED;
      extern const std::string CUSTOM_AUDIO_TRACK_POSITION_REMOVED;
      extern const std::string SOUND_CARD_ADDED;
      extern const std::string SOUND_CARD_CHANGED;
      extern const std::string SOUND_CARD_REMOVED;
      extern const std::string P2P_RESTART;
      extern const std::string P2P_OFFER_SENT;
      extern const std::string P2P_ANSWER_SENT;
      extern const std::string ICE_CANDIDATE_SENT;
    } // namespace RetrieveEvents
  } // namespace DigitalStage

#endif // DS_EVENTS