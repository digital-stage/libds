#ifndef DS_TYPES
#define DS_TYPES

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <set>

using nlohmann::json;

template <class J, class T>
void optional_to_json(J& j, const char* name, const std::optional<T>& value)
{
  if(value) {
    j[name] = *value;
  }
}

template <class J, class T>
void optional_from_json(const J& j, const char* name, std::optional<T>& value)
{
  const auto it = j.find(name);
  if(it != j.end() && !it->is_null()) {
    value = it->template get<T>();
  } else {
    value = std::nullopt;
  }
}

namespace DigitalStage::Types {
  typedef std::string ID_TYPE;
  struct Device {
    ID_TYPE _id;
    ID_TYPE userId;
    std::string uuid;
    std::string type;
    bool online;
    bool canVideo;
    bool canAudio;
    bool sendVideo;
    bool sendAudio;
    bool receiveVideo;
    bool receiveAudio;

    std::set<std::string> availableSoundCardIds;
    std::optional<std::string> soundCardId;

    // jammer specific

    // ov specific
    std::optional<std::string> ovReceiverType; // Either ortf or hrtf
    std::optional<double> ovSenderJitter;
    std::optional<double> ovReceiverJitter;
    std::optional<bool> ovP2p;
    std::optional<bool> ovRenderReverb;
    std::optional<double> ovReverbGain;
    std::optional<bool> ovRenderISM;
    std::optional<bool> ovRawMode;
    std::optional<double> egoGain;
  };

  struct stage_mediasoup_t {
    std::string url;
    uint16_t port;
  };

  struct Stage {
    ID_TYPE _id;
    std::string name;
    std::string description;
    std::optional<std::string> password;
    std::set<std::string> admins;
    std::set<std::string> soundEditors;
    std::optional<std::string> iconUrl;

    std::string videoType;
    std::optional<std::string> videoRouter;
    std::string audioType;
    std::optional<std::string> audioRouter;

    double width;
    double length;
    double height;
    double reflection;
    double absorption;

    // mediasoup specific
    std::optional<stage_mediasoup_t> mediasoup;

    // jammer specific
    std::optional<std::string> jammerIpv4;
    std::optional<std::string> jammerIpv6;
    std::optional<uint16_t> jammerPort;
    std::optional<std::string> jammerKey;

    // ov specific
    std::optional<bool> ovRenderAmbient;
    std::optional<std::string> ovAmbientSoundUrl;
    std::optional<double> ovAmbientLevel;
    std::optional<std::string> ovIpv4;
    std::optional<std::string> ovIpv6;
    std::optional<uint16_t> ovPort;
    std::optional<uint32_t> ovPin;
    std::optional<double> ovJitter;
  };

  struct Group {
    ID_TYPE _id;
    ID_TYPE stageId;
    std::string name;
    std::string description;
    std::optional<std::string> iconUrl;
    std::string color;
    double volume = 1;
    bool muted = false;
    std::string directivity = "cardoid"; // "omni" or "cardoid"
    double x;
    double y;
    double z;
    double rX;
    double rY;
    double rZ;
  };

  struct CustomGroupPosition {
    ID_TYPE _id;
    ID_TYPE userId;
    ID_TYPE deviceId;
    std::string groupId;
    std::string directivity = "cardoid"; // "omni" or "cardoid"
    double x = std::numeric_limits<double>::lowest();
    double y = std::numeric_limits<double>::lowest();
    double z = std::numeric_limits<double>::lowest();
    double rX = std::numeric_limits<double>::lowest();
    double rY = std::numeric_limits<double>::lowest();
    double rZ = std::numeric_limits<double>::lowest();
  };

  struct CustomGroupVolume {
    ID_TYPE _id;
    ID_TYPE userId;
    ID_TYPE deviceId;
    std::string groupId;
    double volume = 1;
    bool muted = false;
  };

  struct StageMember {
    ID_TYPE _id;
    ID_TYPE stageId;
    std::string groupId;
    ID_TYPE userId;
    bool active;
    bool isDirector;

    double volume;
    bool muted;
    std::string directivity = "cardoid"; // "omni" or "cardoid"
    double x;
    double y;
    double z;
    double rX;
    double rY;
    double rZ;
  };

  struct custom_stage_member_position_t {
    ID_TYPE _id;
    ID_TYPE userId;
    ID_TYPE deviceId;
    ID_TYPE stageMemberId;
    std::string directivity = "cardoid"; // "omni" or "cardoid"
    double x = std::numeric_limits<double>::lowest();
    double y = std::numeric_limits<double>::lowest();
    double z = std::numeric_limits<double>::lowest();
    double rX = std::numeric_limits<double>::lowest();
    double rY = std::numeric_limits<double>::lowest();
    double rZ = std::numeric_limits<double>::lowest();
  };
  struct custom_stage_member_volume_t {
    ID_TYPE _id;
    ID_TYPE userId;
    ID_TYPE deviceId;
    ID_TYPE stageMemberId;
    double volume = 1;
    bool muted = false;
  };

  struct StageDevice {
    std::string _id;
    std::string userId;
    std::string deviceId;
    std::string stageId;
    std::string groupId;
    std::string stageMemberId;
    bool active;

    uint8_t order;

    bool sendLocal;

    double volume;
    bool muted;
    std::string directivity = "cardoid"; // "omni" or "cardoid"
    double x;
    double y;
    double z;
    double rX;
    double rY;
    double rZ;
  };

  struct CustomStageDevicePosition {
    std::string _id;
    std::string userId;
    std::string deviceId;
    std::string stageId;
    std::string stageDeviceId;
    std::string directivity = "cardoid"; // "omni" or "cardoid"
    double x = std::numeric_limits<double>::lowest();
    double y = std::numeric_limits<double>::lowest();
    double z = std::numeric_limits<double>::lowest();
    double rX = std::numeric_limits<double>::lowest();
    double rY = std::numeric_limits<double>::lowest();
    double rZ = std::numeric_limits<double>::lowest();
  };

  struct CustomStageDeviceVolume {
    std::string _id;
    std::string userId;
    std::string deviceId;
    std::string stageId;
    std::string stageDeviceId;
    double volume = 1;
    bool muted = false;
  };

  struct SoundCard {
    ID_TYPE _id;
    ID_TYPE userId;
    std::string uuid;

    bool isDefault;

    std::string label;
    std::set<std::string> drivers;
    std::optional<std::string> driver; //'jack' | 'alsa' | 'asio' | 'webrtc'

    double sampleRate;
    std::set<double> sampleRates;
    unsigned int periodSize;
    unsigned int numPeriods;
    std::optional<double> softwareLatency;

    std::map<std::string, bool> inputChannels;
    std::map<std::string, bool> outputChannels;
  };

  struct local_video_track_t {
    ID_TYPE _id;
    ID_TYPE deviceId;
    ID_TYPE userId;
    std::string type;
  };

  struct local_audio_track_t {
    ID_TYPE _id;
    ID_TYPE deviceId;
    ID_TYPE userId;
    std::string type;
    std::optional<std::string> ovSourcePort;
  };

  struct remote_video_track_t {
    ID_TYPE _id;
    ID_TYPE localVideoTrackId;
    ID_TYPE stageMemberId;
    ID_TYPE stageId;
    ID_TYPE userId;
    std::string type;
  };

  struct remote_audio_track_t {
    ID_TYPE _id;
    ID_TYPE localAudioTrackId;
    ID_TYPE deviceId;
    ID_TYPE stageMemberId;
    ID_TYPE stageId;
    ID_TYPE userId;
    std::string type;
    double volume;
    bool muted;
    std::string directivity = "cardoid"; // "omni" or "cardoid"
    double x;
    double y;
    double z;
    double rX;
    double rY;
    double rZ;
    std::optional<std::string> ovSourcePort;
  };

  struct custom_remote_audio_track_position_t {
    ID_TYPE _id;
    ID_TYPE userId;
    ID_TYPE deviceId;
    ID_TYPE remoteAudioTrackId;

    std::string directivity = "cardoid"; // "omni" or "cardoid"
    double x = std::numeric_limits<double>::lowest();
    double y = std::numeric_limits<double>::lowest();
    double z = std::numeric_limits<double>::lowest();
    double rX = std::numeric_limits<double>::lowest();
    double rY = std::numeric_limits<double>::lowest();
    double rZ = std::numeric_limits<double>::lowest();
  };

  struct custom_remote_audio_track_volume_t {
    ID_TYPE _id;
    ID_TYPE userId;
    ID_TYPE deviceId;
    ID_TYPE remoteAudioTrackId;
    double volume = 1;
    bool muted = false;
  };

  struct user_t {
    ID_TYPE _id;
    std::string uid;
    std::string name;
    std::optional<std::string> avatarUrl;
    bool canCreateStage;
    std::optional<ID_TYPE> stageId;
    std::optional<ID_TYPE> groupId;
    std::optional<ID_TYPE> stageMemberId;
  };

  struct WholeStage {
    std::map<std::string, user_t> users;
    std::map<std::string, Device> devices;
    std::map<std::string, SoundCard> soundCards;
    std::map<std::string, Stage> stages;
    std::map<std::string, Group> groups;
    std::map<std::string, std::map<std::string, CustomGroupPosition>> customGroupPositions;
    std::map<std::string, std::map<std::string, CustomGroupVolume>> customGroupVolumes;
    std::map<std::string, StageMember> stageMembers;
    std::map<std::string, std::map<std::string, custom_stage_member_position_t>> customStageMemberPositions;
    std::map<std::string, std::map<std::string, custom_stage_member_volume_t>> customStageMemberVolumes;
    std::map<std::string, StageDevice> stageDevices;
    std::map<std::string, std::map<std::string, CustomStageDevicePosition>> customStageDevicePositions;
    std::map<std::string, std::map<std::string, CustomStageDeviceVolume>> customStageDeviceVolumes;
    std::map<std::string, remote_audio_track_t> remoteAudioTracks;
    std::map<std::string, std::map<std::string, custom_remote_audio_track_position_t>> customRemoteAudioPositions;
    std::map<std::string, std::map<std::string, custom_remote_audio_track_volume_t>> customRemoteAudioVolumes;
    std::map<std::string, remote_video_track_t> remoteVideoTracks;
    std::map<std::string, local_audio_track_t> localAudioTracks;
    std::map<std::string, local_video_track_t> localVideoTracks;
  };

  inline void to_json(json& j, const Device& p)
  {
    j = json{{"_id", p._id},
             {"userId", p.userId},
             {"online", p.online},
             {"uuid", p.uuid},
             {"type", p.type},
             {"canVideo", p.canVideo},
             {"canAudio", p.canAudio},
             {"sendVideo", p.sendVideo},
             {"sendAudio", p.sendAudio},
             {"receiveVideo", p.receiveVideo},
             {"receiveAudio", p.receiveAudio},
             {"availableSoundCardIds", p.availableSoundCardIds}};

    optional_to_json(j, "soundCardId", p.soundCardId);
    optional_to_json(j, "ovReceiverType", p.ovReceiverType);
    optional_to_json(j, "ovSenderJitter", p.ovSenderJitter);
    optional_to_json(j, "ovReceiverJitter", p.ovReceiverJitter);
    optional_to_json(j, "ovP2p", p.ovP2p);
    optional_to_json(j, "ovRenderReverb", p.ovRenderReverb);
    optional_to_json(j, "ovReverbGain", p.ovReverbGain);
    optional_to_json(j, "ovRenderISM", p.ovRenderISM);
    optional_to_json(j, "ovRawMode", p.ovRawMode);
    optional_to_json(j, "egoGain", p.egoGain);
  }

  inline void from_json(const json& j, Device& p)
  {
    j.at("_id").get_to(p._id);
    j.at("userId").get_to(p.userId);
    j.at("uuid").get_to(p.uuid);
    j.at("type").get_to(p.type);
    j.at("online").get_to(p.online);
    j.at("canVideo").get_to(p.canVideo);
    j.at("canAudio").get_to(p.canAudio);
    j.at("sendVideo").get_to(p.sendVideo);
    j.at("sendAudio").get_to(p.sendAudio);
    j.at("receiveVideo").get_to(p.receiveVideo);
    j.at("receiveAudio").get_to(p.receiveAudio);

    if(!j.at("availableSoundCardIds").is_null()) {
      j.at("availableSoundCardIds").get_to(p.availableSoundCardIds);
    } else {
      p.availableSoundCardIds = std::set<std::string>();
    }
    optional_from_json(j, "soundCardId", p.soundCardId);

    // ov specific
    optional_from_json(j, "ovReceiverType", p.ovReceiverType);
    optional_from_json(j, "ovSenderJitter", p.ovSenderJitter);
    optional_from_json(j, "ovReceiverJitter", p.ovReceiverJitter);
    optional_from_json(j, "ovP2p", p.ovP2p);
    optional_from_json(j, "ovRenderReverb", p.ovRenderReverb);
    optional_from_json(j, "ovReverbGain", p.ovReverbGain);
    optional_from_json(j, "ovRenderISM", p.ovRenderISM);
    optional_from_json(j, "ovRawMode", p.ovRawMode);
    optional_from_json(j, "egoGain", p.egoGain);
  }

  inline void to_json(json& j, const stage_mediasoup_t& p)
  {
    j = json{{"url", p.url}, {"port", p.port}};
  }
  inline void from_json(const json& j, stage_mediasoup_t& p)
  {
    j.at("url").get_to(p.url);
    j.at("port").get_to(p.port);
  }

  inline void to_json(json& j, const Stage& p)
  {
    j = json{{"_id", p._id},
             {"name", p.name},
             {"description", p.description},
             {"admins", p.admins},
             {"soundEditors", p.soundEditors},
             {"videoType", p.videoType},
             {"audioType", p.audioType},
             {"width", p.width},
             {"length", p.length},
             {"height", p.height},
             {"absorption", p.absorption},
             {"reflection", p.reflection}};
    optional_to_json(j, "iconUrl", p.iconUrl);
    optional_to_json(j, "password", p.password);
    optional_to_json(j, "videoRouter", p.videoRouter);
    optional_to_json(j, "audioRouter", p.audioRouter);

    optional_to_json(j, "jammerIpv4", p.jammerIpv4);
    optional_to_json(j, "jammerIpv6", p.jammerIpv6);
    optional_to_json(j, "jammerKey", p.jammerKey);
    optional_to_json(j, "jammerPort", p.jammerPort);

    optional_to_json(j, "ovAmbientLevel", p.ovAmbientLevel);
    optional_to_json(j, "ovAmbientSoundUrl", p.ovAmbientSoundUrl);
    optional_to_json(j, "ovIpv4", p.ovIpv4);
    optional_to_json(j, "ovIpv6", p.ovIpv6);
    optional_to_json(j, "ovJitter", p.ovJitter);
    optional_to_json(j, "ovPin", p.ovPin);
    optional_to_json(j, "ovPort", p.ovPort);
    optional_to_json(j, "ovRenderAmbient", p.ovRenderAmbient);
  }

  inline void from_json(const json& j, Stage& p)
  {
    j.at("_id").get_to(p._id);
    j.at("name").get_to(p.name);
    j.at("description").get_to(p.description);
    j.at("admins").get_to(p.admins);
    j.at("soundEditors").get_to(p.soundEditors);
    j.at("videoType").get_to(p.videoType);
    j.at("audioType").get_to(p.audioType);
    j.at("width").get_to(p.width);
    j.at("length").get_to(p.length);
    j.at("height").get_to(p.height);
    j.at("absorption").get_to(p.absorption);
    j.at("reflection").get_to(p.reflection);
    optional_from_json(j, "iconUrl", p.iconUrl);
    optional_from_json(j, "password", p.password);
    optional_from_json(j, "videoRouter", p.videoRouter);
    optional_from_json(j, "audioRouter", p.audioRouter);

    if(j.count("mediasoup") != 0) {
      p.mediasoup = j.at("mediasoup").get<stage_mediasoup_t>();
    } else {
      p.mediasoup = std::nullopt;
    }

    optional_from_json(j, "jammerIpv4", p.jammerIpv4);
    optional_from_json(j, "jammerIpv6", p.jammerIpv6);
    optional_from_json(j, "jammerKey", p.jammerKey);
    optional_from_json(j, "jammerPort", p.jammerPort);

    optional_from_json(j, "ovAmbientLevel", p.ovAmbientLevel);
    optional_from_json(j, "ovAmbientSoundUrl", p.ovAmbientSoundUrl);
    optional_from_json(j, "ovIpv4", p.ovIpv4);
    optional_from_json(j, "ovIpv6", p.ovIpv6);
    optional_from_json(j, "ovJitter", p.ovJitter);
    optional_from_json(j, "ovPin", p.ovPin);
    optional_from_json(j, "ovPort", p.ovPort);
    optional_from_json(j, "ovRenderAmbient", p.ovRenderAmbient);
  }

  inline void to_json(json& j, const Group& p)
  {
    j = json{{"_id", p._id},     {"stageId", p.stageId},
             {"name", p.name},   {"description", p.description},
             {"color", p.color}, {"volume", p.volume},
             {"muted", p.muted}, {"x", p.x},
             {"y", p.y},         {"z", p.z},
             {"rX", p.rX},       {"rY", p.rY},
             {"rZ", p.rZ}};
    optional_to_json(j, "iconUrl", p.iconUrl);
  }

  inline void from_json(const json& j, Group& p)
  {
    j.at("_id").get_to(p._id);
    j.at("stageId").get_to(p.stageId);
    j.at("name").get_to(p.name);
    j.at("description").get_to(p.description);
    j.at("color").get_to(p.color);
    j.at("volume").get_to(p.volume);
    j.at("muted").get_to(p.muted);
    j.at("x").get_to(p.x);
    j.at("y").get_to(p.y);
    j.at("z").get_to(p.z);
    j.at("rX").get_to(p.rX);
    j.at("rY").get_to(p.rY);
    j.at("rZ").get_to(p.rZ);
    optional_from_json(j, "iconUrl", p.iconUrl);
  }

  inline void to_json(json& j, const CustomGroupPosition& p)
  {
    j = json{{"_id", p._id},
             {"userId", p.userId},
             {"deviceId", p.deviceId},
             {"groupId", p.groupId},
             {"x", p.x},
             {"y", p.y},
             {"z", p.z},
             {"rX", p.rX},
             {"rY", p.rY},
             {"rZ", p.rZ}};
  }

  inline void from_json(const json& j, CustomGroupPosition& p)
  {
    j.at("_id").get_to(p._id);
    j.at("userId").get_to(p.userId);
    j.at("deviceId").get_to(p.deviceId);
    j.at("groupId").get_to(p.groupId);
    j.at("x").get_to(p.x);
    j.at("y").get_to(p.y);
    j.at("z").get_to(p.z);
    j.at("rX").get_to(p.rX);
    j.at("rY").get_to(p.rY);
    j.at("rZ").get_to(p.rZ);
  }

  inline void to_json(json& j, const CustomGroupVolume& p)
  {
    j = json{{"_id", p._id},           {"userId", p.userId},
             {"deviceId", p.deviceId}, {"groupId", p.groupId},
             {"volume", p.volume},     {"muted", p.muted}};
  }

  inline void from_json(const json& j, CustomGroupVolume& p)
  {
    j.at("_id").get_to(p._id);
    j.at("userId").get_to(p.userId);
    j.at("deviceId").get_to(p.deviceId);
    j.at("groupId").get_to(p.groupId);
    j.at("volume").get_to(p.volume);
    j.at("muted").get_to(p.muted);
  }

  inline void to_json(json& j, const StageMember& p)
  {
    j = json{{"_id", p._id},
             {"stageId", p.stageId},
             {"groupId", p.groupId},
             {"userId", p.userId},
             {"active", p.active},
             {"isDirector", p.isDirector},
             {"volume", p.volume},
             {"muted", p.muted},
             {"x", p.x},
             {"y", p.y},
             {"z", p.z},
             {"rX", p.rX},
             {"rY", p.rY},
             {"rZ", p.rZ}};
  }

  inline void from_json(const json& j, StageMember& p)
  {
    j.at("_id").get_to(p._id);
    j.at("stageId").get_to(p.stageId);
    j.at("groupId").get_to(p.groupId);
    j.at("userId").get_to(p.userId);
    j.at("active").get_to(p.active);
    j.at("isDirector").get_to(p.isDirector);
    j.at("volume").get_to(p.volume);
    j.at("muted").get_to(p.muted);
    j.at("x").get_to(p.x);
    j.at("y").get_to(p.y);
    j.at("z").get_to(p.z);
    j.at("rX").get_to(p.rX);
    j.at("rY").get_to(p.rY);
    j.at("rZ").get_to(p.rZ);
  }

  inline void to_json(json& j, const custom_stage_member_position_t& p)
  {
    j = json{{"_id", p._id},       {"deviceId", p.deviceId},
             {"userId", p.userId}, {"stageMemberId", p.stageMemberId},
             {"x", p.x},           {"y", p.y},
             {"z", p.z},           {"rX", p.rX},
             {"rY", p.rY},         {"rZ", p.rZ}};
  }

  inline void from_json(const json& j, custom_stage_member_position_t& p)
  {
    j.at("_id").get_to(p._id);
    j.at("userId").get_to(p.userId);
    j.at("deviceId").get_to(p.deviceId);
    j.at("stageId").get_to(p.deviceId);
    j.at("stageMemberId").get_to(p.stageMemberId);
    j.at("x").get_to(p.x);
    j.at("y").get_to(p.y);
    j.at("z").get_to(p.z);
    j.at("rX").get_to(p.rX);
    j.at("rY").get_to(p.rY);
    j.at("rZ").get_to(p.rZ);
  }

  inline void to_json(json& j, const custom_stage_member_volume_t& p)
  {
    j = json{{"_id", p._id},           {"userId", p.userId},
             {"deviceId", p.deviceId}, {"stageMemberId", p.stageMemberId},
             {"volume", p.volume},     {"muted", p.muted}};
  }

  inline void from_json(const json& j, custom_stage_member_volume_t& p)
  {
    j.at("_id").get_to(p._id);
    j.at("userId").get_to(p.userId);
    j.at("stageMemberId").get_to(p.stageMemberId);
    j.at("volume").get_to(p.volume);
    j.at("muted").get_to(p.muted);
  }

  inline void to_json(json& j, const StageDevice& p)
  {
    j = json{{"_id", p._id},
             {"userId", p.userId},
             {"deviceId", p.deviceId},
             {"stageId", p.stageId},
             {"groupId", p.groupId},
             {"stageMemberId", p.stageMemberId},
             {"active", p.active},
             {"order", p.order},
             {"sendLocal", p.sendLocal},
             {"volume", p.volume},
             {"muted", p.muted},
             {"x", p.x},
             {"y", p.y},
             {"z", p.z},
             {"rX", p.rX},
             {"rY", p.rY},
             {"rZ", p.rZ}};
  }

  inline void from_json(const json& j, StageDevice& p)
  {
    j.at("_id").get_to(p._id);
    j.at("userId").get_to(p.userId);
    j.at("deviceId").get_to(p.deviceId);
    j.at("stageId").get_to(p.stageId);
    j.at("groupId").get_to(p.groupId);
    j.at("stageMemberId").get_to(p.stageMemberId);
    j.at("active").get_to(p.active);
    j.at("order").get_to(p.order);
    j.at("sendLocal").get_to(p.sendLocal);
    j.at("volume").get_to(p.volume);
    j.at("muted").get_to(p.muted);
    j.at("x").get_to(p.x);
    j.at("y").get_to(p.y);
    j.at("z").get_to(p.z);
    j.at("rX").get_to(p.rX);
    j.at("rY").get_to(p.rY);
    j.at("rZ").get_to(p.rZ);
  }

  inline void to_json(json& j, const CustomStageDevicePosition& p)
  {
    j = json{{"_id", p._id},
             {"deviceId", p.deviceId},
             {"userId", p.userId},
             {"stageId", p.stageId},
             {"stageDeviceId", p.stageDeviceId},
             {"x", p.x},
             {"y", p.y},
             {"z", p.z},
             {"rX", p.rX},
             {"rY", p.rY},
             {"rZ", p.rZ}};
  }

  inline void from_json(const json& j, CustomStageDevicePosition& p)
  {
    j.at("_id").get_to(p._id);
    j.at("userId").get_to(p.userId);
    j.at("deviceId").get_to(p.deviceId);
    j.at("stageId").get_to(p.stageId);
    j.at("stageDeviceId").get_to(p.stageDeviceId);
    j.at("x").get_to(p.x);
    j.at("y").get_to(p.y);
    j.at("z").get_to(p.z);
    j.at("rX").get_to(p.rX);
    j.at("rY").get_to(p.rY);
    j.at("rZ").get_to(p.rZ);
  }

  inline void to_json(json& j, const CustomStageDeviceVolume& p)
  {
    j = json{{"_id", p._id},
             {"userId", p.userId},
             {"deviceId", p.deviceId},
             {"stageId", p.stageId},
             {"stageDeviceId", p.stageDeviceId},
             {"volume", p.volume},
             {"muted", p.muted}};
  }

  inline void from_json(const json& j, CustomStageDeviceVolume& p)
  {
    j.at("_id").get_to(p._id);
    j.at("userId").get_to(p.userId);
    j.at("stageId").get_to(p.stageId);
    j.at("stageDeviceId").get_to(p.stageDeviceId);
    j.at("volume").get_to(p.volume);
    j.at("muted").get_to(p.muted);
  }

  inline void to_json(json& j, const SoundCard& p)
  {
    j = json{{"_id", p._id},
             {"userId", p.userId},
             {"uuid", p.uuid},
             {"isDefault", p.isDefault},
             {"label", p.label},
             {"drivers", p.drivers},
             {"sampleRate", p.sampleRate},
             {"sampleRates", p.sampleRates},
             {"periodSize", p.periodSize},
             {"numPeriods", p.numPeriods},
             {"inputChannels", p.inputChannels},
             {"outputChannels", p.outputChannels}};
    optional_to_json(j, "driver", p.driver);
    optional_to_json(j, "softwareLatency", p.softwareLatency);
  }

  inline void from_json(const json& j, SoundCard& p)
  {
    j.at("_id").get_to(p._id);
    j.at("userId").get_to(p.userId);
    j.at("uuid").get_to(p.uuid);
    j.at("isDefault").get_to(p.isDefault);
    j.at("label").get_to(p.label);
    j.at("drivers").get_to(p.drivers);
    j.at("sampleRate").get_to(p.sampleRate);
    j.at("sampleRates").get_to(p.sampleRates);
    j.at("periodSize").get_to(p.periodSize);
    j.at("numPeriods").get_to(p.numPeriods);
    j.at("inputChannels").get_to(p.inputChannels);
    j.at("outputChannels").get_to(p.outputChannels);
    optional_from_json(j, "driver", p.driver);
    optional_from_json(j, "softwareLatency", p.softwareLatency);
  }

  inline void to_json(json& j, const local_video_track_t& p)
  {
    j = json{{"_id", p._id},
             {"userId", p.userId},
             {"deviceId", p.deviceId},
             {"type", p.type}};
  }

  inline void from_json(const json& j, local_video_track_t& p)
  {
    j.at("_id").get_to(p._id);
    j.at("userId").get_to(p.userId);
    j.at("deviceId").get_to(p.deviceId);
    j.at("type").get_to(p.type);
  }

  inline void to_json(json& j, const local_audio_track_t& p)
  {
    j = json{{"_id", p._id},
             {"userId", p.userId},
             {"deviceId", p.deviceId},
             {"type", p.type}};
    optional_to_json(j, "ovSourcePort", p.ovSourcePort);
  }

  inline void from_json(const json& j, local_audio_track_t& p)
  {
    j.at("_id").get_to(p._id);
    j.at("userId").get_to(p.userId);
    j.at("deviceId").get_to(p.deviceId);
    j.at("type").get_to(p.type);
    optional_from_json(j, "ovSourcePort", p.ovSourcePort);
  }

  inline void to_json(json& j, const remote_video_track_t& p)
  {
    j = json{{"_id", p._id},
             {"localVideoTrackId", p.localVideoTrackId},
             {"stageMemberId", p.stageMemberId},
             {"stageId", p.stageId},
             {"userId", p.userId},
             {"type", p.type}};
  }

  inline void from_json(const json& j, remote_video_track_t& p)
  {
    j.at("_id").get_to(p._id);
    j.at("localVideoTrackId").get_to(p.localVideoTrackId);
    j.at("stageMemberId").get_to(p.stageMemberId);
    j.at("stageId").get_to(p.stageId);
    j.at("userId").get_to(p.userId);
    j.at("type").get_to(p.type);
  }

  inline void to_json(json& j, const remote_audio_track_t& p)
  {
    j = json{{"_id", p._id},
             {"localAudioTrackId", p.localAudioTrackId},
             {"stageMemberId", p.stageMemberId},
             {"stageId", p.stageId},
             {"userId", p.userId},
             {"type", p.type},
             {"volume", p.volume},
             {"muted", p.muted},
             {"directivity", p.directivity},
             {"x", p.x},
             {"y", p.y},
             {"z", p.z},
             {"rX", p.rX},
             {"rY", p.rY},
             {"rZ", p.rZ}};
    optional_to_json(j, "ovSourcePort", p.ovSourcePort);
  }

  inline void from_json(const json& j, remote_audio_track_t& p)
  {
    j.at("_id").get_to(p._id);
    j.at("localAudioTrackId").get_to(p.localAudioTrackId);
    j.at("stageMemberId").get_to(p.stageMemberId);
    j.at("deviceId").get_to(p.deviceId);
    j.at("stageId").get_to(p.stageId);
    j.at("userId").get_to(p.userId);
    j.at("type").get_to(p.type);
    j.at("volume").get_to(p.volume);
    j.at("muted").get_to(p.muted);
    j.at("directivity").get_to(p.directivity);
    j.at("x").get_to(p.x);
    j.at("y").get_to(p.y);
    j.at("z").get_to(p.z);
    j.at("rX").get_to(p.rX);
    j.at("rY").get_to(p.rY);
    j.at("rZ").get_to(p.rZ);
    optional_from_json(j, "ovSourcePort", p.ovSourcePort);
  }

  inline void to_json(json& j, const custom_remote_audio_track_position_t& p)
  {
    j = json{{"_id", p._id},
             {"userId", p.userId},
             {"deviceId", p.deviceId},
             {"remoteAudioTrackId", p.remoteAudioTrackId},
             {"directivity", p.directivity},
             {"x", p.x},
             {"y", p.y},
             {"z", p.z},
             {"rX", p.rX},
             {"rY", p.rY},
             {"rZ", p.rZ}};
  }

  inline void from_json(const json& j, custom_remote_audio_track_position_t& p)
  {
    j.at("_id").get_to(p._id);
    j.at("userId").get_to(p.userId);
    j.at("deviceId").get_to(p.deviceId);
    j.at("remoteAudioTrackId").get_to(p.remoteAudioTrackId);
    j.at("directivity").get_to(p.directivity);
    j.at("y").get_to(p.y);
    j.at("z").get_to(p.z);
    j.at("rX").get_to(p.rX);
    j.at("rY").get_to(p.rY);
    j.at("rZ").get_to(p.rZ);
  }

  inline void to_json(json& j, const custom_remote_audio_track_volume_t& p)
  {
    j = json{
        {"_id", p._id},           {"userId", p.userId},
        {"deviceId", p.deviceId}, {"remoteAudioTrackId", p.remoteAudioTrackId},
        {"volume", p.volume},     {"muted", p.muted}};
  }

  inline void from_json(const json& j, custom_remote_audio_track_volume_t& p)
  {
    j.at("_id").get_to(p._id);
    j.at("userId").get_to(p.userId);
    j.at("deviceId").get_to(p.deviceId);
    j.at("remoteAudioTrackId").get_to(p.remoteAudioTrackId);
    j.at("volume").get_to(p.volume);
    j.at("muted").get_to(p.muted);
  }

  inline void to_json(json& j, const user_t& p)
  {
    j = json{{"_id", p._id},
             {"uid", p.uid},
             {"name", p.name},
             {"canCreateStage", p.canCreateStage}};
    optional_to_json(j, "avatarUrl", p.avatarUrl);
    optional_to_json(j, "stageId", p.stageId);
    optional_to_json(j, "groupId", p.groupId);
    optional_to_json(j, "stageMemberId", p.stageMemberId);
  }

  inline void from_json(const json& j, user_t& p)
  {
    j.at("_id").get_to(p._id);
    j.at("uid").get_to(p.uid);
    j.at("name").get_to(p.name);
    j.at("canCreateStage").get_to(p.canCreateStage);
    optional_from_json(j, "avatarUrl", p.avatarUrl);
    optional_from_json(j, "stageId", p.stageId);
    optional_from_json(j, "groupId", p.groupId);
    optional_from_json(j, "stageMemberId", p.stageMemberId);
  }

  inline void to_json(json& j, const WholeStage& p)
  {
    j = json{
        {"users", p.users},
        {"devices", p.devices},
        {"soundCards", p.soundCards},
        {"stages", p.stages},
        {"groups", p.groups},
        {"customGroupVolumes", p.customGroupVolumes},
        {"customGroupVolumes", p.customGroupPositions},
        {"stageMembers", p.stageMembers},
        {"customStageMemberVolumes", p.customStageMemberVolumes},
        {"customStageMemberPositions", p.customStageMemberPositions},
        {"stageDevices", p.stageDevices},
        {"customStageMemberVolumes", p.customStageMemberVolumes},
        {"customStageMemberPositions", p.customStageMemberPositions},
        {"remoteAudioTracks", p.remoteAudioTracks},
        {"customRemoteAudioPositions", p.customRemoteAudioPositions},
        {"customRemoteAudioVolumes", p.customRemoteAudioVolumes},
        {"remoteVideoTracks", p.remoteVideoTracks},
        {"localAudioTracks", p.localAudioTracks},
        {"localVideoTracks", p.localVideoTracks}
    };
  }

  inline void from_json(const json& j, WholeStage& p)
  {
    j.at("users").get_to(p.users);
    j.at("devices").get_to(p.devices);
    j.at("soundCards").get_to(p.soundCards);
    j.at("stages").get_to(p.stages);
    j.at("groups").get_to(p.groups);
    j.at("customGroupVolumes").get_to(p.customGroupVolumes);
    j.at("customGroupVolumes").get_to(p.customGroupPositions);
    j.at("stageMembers").get_to(p.stageMembers);
    j.at("customStageMemberVolumes").get_to(p.customStageMemberVolumes);
    j.at("customStageMemberPositions").get_to(p.customStageMemberPositions);
    j.at("stageDevices").get_to(p.stageDevices);
    j.at("customStageMemberVolumes").get_to(p.customStageMemberVolumes);
    j.at("customStageMemberPositions").get_to(p.customStageMemberPositions);
    j.at("remoteAudioTracks").get_to(p.remoteAudioTracks);
    j.at("customRemoteAudioPositions").get_to(p.customRemoteAudioPositions);
    j.at("customRemoteAudioVolumes").get_to(p.customRemoteAudioVolumes);
    j.at("remoteVideoTracks").get_to(p.remoteVideoTracks);
    j.at("localAudioTracks").get_to(p.localAudioTracks);
    j.at("localVideoTracks").get_to(p.localVideoTracks);
  }
} // namespace DigitalStage::Types

#endif