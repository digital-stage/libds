#ifndef DS_EVENTS
#define DS_EVENTS

#include <DigitalStage/Types.h>
#include <nlohmann/json.hpp>
#include <string>

namespace DigitalStage {
  namespace Api {   
    namespace SendEvents {
      extern const std::string CHANGE_DEVICE;
      extern const std::string SET_SOUND_CARD;
      extern const std::string CHANGE_SOUND_CARD;
      extern const std::string CREATE_AUDIO_TRACK;
      extern const std::string SET_AUDIO_TRACK;
      extern const std::string CHANGE_AUDIO_TRACK;
      extern const std::string REMOVE_AUDIO_TRACK;
      extern const std::string SEND_P2P_RESTART;
      extern const std::string SEND_P2P_OFFER;
      extern const std::string SEND_P2P_ANSWER;
      extern const std::string SEND_ICE_CANDIDATE;

      extern const std::string CREATE_STAGE;
      extern const std::string CHANGE_STAGE;
      extern const std::string REMOVE_STAGE;

      extern const std::string CREATE_GROUP;
      extern const std::string CHANGE_GROUP;
      extern const std::string REMOVE_GROUP;

      extern const std::string JOIN_STAGE;
      extern const std::string LEAVE_STAGE;
      extern const std::string LEAVE_STAGE_FOR_GOOD;

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
      extern const std::string USER_ADDED;
      extern const std::string USER_CHANGED;
      extern const std::string USER_REMOVED;
      extern const std::string GROUP_ADDED;
      extern const std::string GROUP_CHANGED;
      extern const std::string GROUP_REMOVED;
      extern const std::string STAGE_MEMBER_ADDED;
      extern const std::string STAGE_MEMBER_CHANGED;
      extern const std::string STAGE_MEMBER_REMOVED;
      extern const std::string STAGE_DEVICE_ADDED;
      extern const std::string STAGE_DEVICE_CHANGED;
      extern const std::string STAGE_DEVICE_REMOVED;
      extern const std::string CUSTOM_GROUP_ADDED;
      extern const std::string CUSTOM_GROUP_CHANGED;
      extern const std::string CUSTOM_GROUP_REMOVED;
      extern const std::string VIDEO_TRACK_ADDED;
      extern const std::string VIDEO_TRACK_CHANGED;
      extern const std::string VIDEO_TRACK_REMOVED;
      extern const std::string AUDIO_TRACK_ADDED;
      extern const std::string AUDIO_TRACK_CHANGED;
      extern const std::string AUDIO_TRACK_REMOVED;
      extern const std::string SOUND_CARD_ADDED;
      extern const std::string SOUND_CARD_CHANGED;
      extern const std::string SOUND_CARD_REMOVED;
      extern const std::string P2P_RESTART;
      extern const std::string P2P_OFFER_SENT;
      extern const std::string P2P_ANSWER_SENT;
      extern const std::string ICE_CANDIDATE_SENT;
      extern const std::string TURN_SERVERS_CHANGED;
    } // namespace RetrieveEvents
  }   // namespace Api
} // namespace DigitalStage

#endif // DS_EVENTS
