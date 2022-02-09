#ifndef DS_TYPES
#define DS_TYPES

#include <iostream>
#include <nlohmann/json.hpp>
#include <set>
#include <string>
#include <optional>

namespace DigitalStage::Types {
using nlohmann::json;

class ParseException : public std::runtime_error {
 public:
  explicit ParseException(const std::string &what) : std::runtime_error(what) {}
};

template<typename ValueType>
void required_from_json(const nlohmann::json &json, const std::string &key, ValueType &target) {
  if (!json.contains(key)) {
    throw DigitalStage::Types::ParseException("Missing key '" + key + "'");
  }
  try {
    json.at(key).get_to(target);
  } catch (const nlohmann::json::exception &e) {
    throw DigitalStage::Types::ParseException(key + ": " + e.what());
  }
}

template<class J, class T>
void optional_from_json(const J &j, const char *name, std::optional<T> &target) {
  const auto it = j.find(name);
  if (it != j.end() && !it->is_null()) {
    target = it->template get<T>();
  } else {
    target = std::nullopt;
  }
}

template<class J, class T>
void optional_to_json(J &j, const char *name, const std::optional<T> &value) {
  if (value) {
    j[name] = *value;
  }
}

using ID_TYPE = std::string;

struct Device {
  ID_TYPE _id;
  ID_TYPE userId;
  /**
   * Use this to identify existing devices on connect, on native clients you might use the mac address, on browser a cookie _id
   */
  std::string uuid;
  std::string type;
  bool online;
  bool canVideo;
  bool canAudio;
  bool sendVideo;
  bool sendAudio;
  bool receiveVideo;
  bool receiveAudio;

  /**
   * The currently selected audioDriver (only one allowed for input and output sound cards)
   */
  std::optional<std::string> audioDriver;
  /**
   * Audio engine currently selected (will be "juce" always for Jammer and "webaudio" always for "browser").
   * This enables us to change the audio engine on the fly later.
   */
  std::optional<std::string> audioEngine;
  /**
   * The currently selected input sound card or null if none selected
   */
  std::optional<ID_TYPE> inputSoundCardId;
  /**
   * The currently selected output sound card or null if none selected
   */
  std::optional<ID_TYPE> outputSoundCardId;

  /**
   * The amount of network receiver buffer in samples
   */
  unsigned int buffer;

  /**
   * The output volume
   */
  double volume;
  /**
   * The balance between the monitor and the session mixer aka Ego balance
   */
  double balance;

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

struct ThreeDimensionalProperties {
  std::string directivity = "cardoid"; // "omni" or "cardoid"
  double x = 0;
  double y = -1;
  double z = 0;
  double rX = 0;
  double rY = 0;
  double rZ = -180;
};

struct VolumeProperties {
  double volume = 1;
  bool muted = false;
};

struct Group : VolumeProperties, ThreeDimensionalProperties {
  ID_TYPE _id;
  ID_TYPE stageId;
  std::string name;
  std::string description;
  std::optional<std::string> iconUrl;
  std::string color;
};

struct CustomGroup : VolumeProperties, ThreeDimensionalProperties {
  ID_TYPE _id;
  ID_TYPE groupId;
  ID_TYPE targetGroupId;
  ID_TYPE stageId;
};

struct StageMember : VolumeProperties, ThreeDimensionalProperties {
  ID_TYPE _id;
  ID_TYPE stageId;
  ID_TYPE userId;
  std::optional<ID_TYPE> groupId;
  std::string name;
  bool active;
  bool isDirector;
};

struct StageDevice {
  ID_TYPE _id;
  ID_TYPE userId;
  ID_TYPE deviceId;
  ID_TYPE stageId;
  std::optional<ID_TYPE> groupId;
  ID_TYPE stageMemberId;
  bool active;
  std::string type;

  uint8_t order;

  bool sendLocal;
};

struct Channel {
  /**
   * Optional label of this channel.
   * If no label is given, the web client will display the channel index instead
   */
  std::optional<std::string> label;
  /**
   * Indicates, if this channel is or shall be currently active
   */
  bool active;
};

/**
 * A soundcard is unique by the combination of
 *  - audioDriver
 *  - label
 *  - deviceId
 *  - type
 * and unique of course by its single _id
 *
 *
 */
struct SoundCard {
  ID_TYPE _id;
  ID_TYPE deviceId;

  /**
   * Audio engine, e.g. "juce", "rtaudio", "miniaudio", "webaudio"
   */
  std::string audioEngine;

  /**
   * Audio driver, inside JUCE use getTypeName of https://docs.juce.com/master/classAudioIODevice.html
   */
  std::string audioDriver;

  /**
   * Direction or type of this sound card, valid values are 'input' or 'output'
   */
  std::string type;
  /**
   * Internal UUID of this sound card, inside JUCE use getName of https://docs.juce.com/master/classAudioIODevice.html
   */
  std::string uuid;
  /**
   * Label of this sound card, usually unique per client device in combination with the audioDriver,
   * inside JUCE this is the same as the uuid
   */
  std::string label;

  /**
   * Indicates, if this device is system default
   */
  std::optional<bool> isDefault;

  /**
   * The frame size or buffer size used by this sound card (may be ignored by the audio engine).
   */
  unsigned int bufferSize;

  unsigned int sampleRate;
  std::set<unsigned int> sampleRates;
  unsigned int periodSize;
  unsigned int numPeriods = 2;
  std::optional<double> softwareLatency;

  /**
   * Indicate if the given sound card is connected and online
   */
  bool online;

  /**
   * Indexed list of channels
   */
  std::vector<Channel> channels;

  // helper entry
  ID_TYPE userId;
};

struct VideoTrack {
  ID_TYPE _id;
  ID_TYPE userId;
  ID_TYPE deviceId;
  ID_TYPE stageId;
  ID_TYPE stageMemberId;
  ID_TYPE stageDeviceId;
  std::string type;
};

struct AudioTrack : VolumeProperties, ThreeDimensionalProperties {
  ID_TYPE _id;
  ID_TYPE userId;
  ID_TYPE deviceId;
  ID_TYPE stageId;
  ID_TYPE stageMemberId;
  ID_TYPE stageDeviceId;
  std::string type;

  /**
   * Optional UUID of this audioTrack, used by DigitalStagePC to match published audio tracks with internal channel IDs
   */
  std::optional<std::string> uuid;
  /**
   * The channel on the source device (see deviceId)
   */
  std::optional<unsigned int> sourceChannel;
  /**
   * Channel (used by our PC client for uniqueness)
   */
  std::optional<std::string> name;
  std::optional<std::string> ovSourcePort;
};

struct User {
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
  std::map<ID_TYPE, User> users;
  std::map<ID_TYPE, Device> devices;
  std::map<ID_TYPE, SoundCard> soundCards;
  std::map<ID_TYPE, Stage> stages;
  std::map<ID_TYPE, Group> groups;
  std::map<ID_TYPE, CustomGroup> customGroups;
  std::map<ID_TYPE, StageMember> stageMembers;
  std::map<ID_TYPE, StageDevice> stageDevices;
  std::map<ID_TYPE, AudioTrack> audioTracks;
  std::map<ID_TYPE, VideoTrack> videoTracks;
};

struct SessionDescriptionInit {
  std::string sdp;
  std::string type;
};

struct P2PRestart {
  ID_TYPE from;
  ID_TYPE to;
};

struct P2POffer {
  ID_TYPE from;
  ID_TYPE to;
  SessionDescriptionInit offer;
};

struct P2PAnswer {
  ID_TYPE from;
  ID_TYPE to;
  SessionDescriptionInit answer;
};

struct IceCandidateInit {
  std::string candidate;
  int sdpMLineIndex;
  std::string sdpMid;
};

struct IceCandidate {
  ID_TYPE from;
  ID_TYPE to;
  std::optional<IceCandidateInit> iceCandidate;
};

inline void to_json(json &j, const ThreeDimensionalProperties &p) {
  j = json{{"x", p.x},
           {"y", p.y},
           {"z", p.z},
           {"rX", p.rX},
           {"rY", p.rY},
           {"rZ", p.rZ},
           {"directivity", p.directivity}};
}

inline void from_json(const json &j, ThreeDimensionalProperties &p) {
  if (j.contains("x") && j.at("x").is_number()) {
    required_from_json(j, "x", p.x);
  }
  if (j.contains("y") && j.at("y").is_number()) {
    required_from_json(j, "y", p.y);
  }
  if (j.contains("z") && j.at("z").is_number()) {
    required_from_json(j, "z", p.z);
  }
  if (j.contains("rX") && j.at("rX").is_number()) {
    required_from_json(j, "rX", p.rX);
  }
  if (j.contains("rY") && j.at("rY").is_number()) {
    required_from_json(j, "rY", p.rY);
  }
  if (j.contains("rZ") && j.at("rZ").is_number()) {
    required_from_json(j, "rZ", p.rZ);
  }
  if (j.contains("directivity") && j.at("directivity").is_string()) {
    required_from_json(j, "directivity", p.directivity);
  }
}

inline void to_json(json &j, const VolumeProperties &p) {
  j = json{{"volume", p.volume},
           {"muted", p.muted}};
}

inline void from_json(const json &j, VolumeProperties &p) {
  if (j.contains("volume") && j.at("volume").is_number()) {
    required_from_json(j, "volume", p.volume);
  }
  if (j.contains("muted") && j.at("muted").is_number()) {
    required_from_json(j, "muted", p.muted);
  }
}

inline void to_json(json &j, const Device &p) {
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
           {"volume", p.volume},
           {"balance", p.balance},
           {"buffer", p.buffer}};
  optional_to_json(j, "audioDriver", p.audioDriver);
  optional_to_json(j, "audioEngine", p.audioEngine);
  optional_to_json(j, "inputSoundCardId", p.inputSoundCardId);
  optional_to_json(j, "outputSoundCardId", p.outputSoundCardId);
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

inline void from_json(const json &j, Device &p) {
  required_from_json(j, "_id", p._id);
  required_from_json(j, "userId", p.userId);
  required_from_json(j, "uuid", p.uuid);
  required_from_json(j, "type", p.type);
  required_from_json(j, "online", p.online);
  required_from_json(j, "canVideo", p.canVideo);
  required_from_json(j, "canAudio", p.canAudio);
  required_from_json(j, "sendVideo", p.sendVideo);
  required_from_json(j, "sendAudio", p.sendAudio);
  required_from_json(j, "receiveVideo", p.receiveVideo);
  required_from_json(j, "receiveAudio", p.receiveAudio);
  required_from_json(j, "volume", p.volume);
  required_from_json(j, "balance", p.balance);

  optional_from_json(j, "audioDriver", p.audioDriver);
  optional_from_json(j, "audioEngine", p.audioEngine);
  optional_from_json(j, "inputSoundCardId", p.inputSoundCardId);
  optional_from_json(j, "outputSoundCardId", p.outputSoundCardId);

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

inline void to_json(json &j, const stage_mediasoup_t &p) {
  j = json{{"url", p.url}, {"port", p.port}};
}
inline void from_json(const json &j, stage_mediasoup_t &p) {
  required_from_json(j, "url", p.url);
  required_from_json(j, "port", p.port);
}

inline void to_json(json &j, const Stage &p) {
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

inline void from_json(const json &j, Stage &p) {
  required_from_json(j, "_id", p._id);
  required_from_json(j, "name", p.name);
  required_from_json(j, "description", p.description);
  required_from_json(j, "admins", p.admins);
  required_from_json(j, "soundEditors", p.soundEditors);
  required_from_json(j, "videoType", p.videoType);
  required_from_json(j, "audioType", p.audioType);
  required_from_json(j, "width", p.width);
  required_from_json(j, "length", p.length);
  required_from_json(j, "height", p.height);
  required_from_json(j, "absorption", p.absorption);
  required_from_json(j, "reflection", p.reflection);
  optional_from_json(j, "iconUrl", p.iconUrl);
  optional_from_json(j, "password", p.password);
  optional_from_json(j, "videoRouter", p.videoRouter);
  optional_from_json(j, "audioRouter", p.audioRouter);

  if (j.count("mediasoup") != 0) {
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

inline void to_json(json &j, const Group &p) {
  j = json{{"_id", p._id}, {"stageId", p.stageId}, {"name", p.name}, {"description", p.description}, {"color", p.color},
           {"volume", p.volume}, {"muted", p.muted}, {"x", p.x}, {"y", p.y}, {"z", p.z}, {"rX", p.rX}, {"rY", p.rY},
           {"rZ", p.rZ}};
  optional_to_json(j, "iconUrl", p.iconUrl);
}

inline void from_json(const json &j, Group &p) {
  required_from_json(j, "_id", p._id);
  required_from_json(j, "stageId", p.stageId);
  required_from_json(j, "name", p.name);
  required_from_json(j, "description", p.description);
  required_from_json(j, "color", p.color);
  optional_from_json(j, "iconUrl", p.iconUrl);
  from_json(j, static_cast<VolumeProperties &>(p));
  from_json(j, static_cast<ThreeDimensionalProperties &>(p));
}

inline void to_json(json &j, const CustomGroup &p) {
  j = json{{"_id", p._id},
           {"groupId", p.groupId},
           {"targetGroupId", p.targetGroupId},
           {"stageId", p.stageId},
           {"volume", p.volume},
           {"muted", p.muted},
           {"x", p.x},
           {"y", p.y},
           {"z", p.z},
           {"rX", p.rX},
           {"rY", p.rY},
           {"rZ", p.rZ},
           {"directivity", p.directivity}};
}

inline void from_json(const json &j, CustomGroup &p) {
  required_from_json(j, "_id", p._id);
  required_from_json(j, "groupId", p.groupId);
  required_from_json(j, "targetGroupId", p.targetGroupId);
  required_from_json(j, "stageId", p.stageId);
  from_json(j, static_cast<VolumeProperties &>(p));
  from_json(j, static_cast<ThreeDimensionalProperties &>(p));
}

inline void to_json(json &j, const StageMember &p) {
  j = json{{"_id", p._id},
           {"stageId", p.stageId},
           {"userId", p.userId},
           {"active", p.active},
           {"isDirector", p.isDirector},
           {"volume", p.volume},
           {"muted", p.muted},
           {"name", p.name},
           {"x", p.x},
           {"y", p.y},
           {"z", p.z},
           {"rX", p.rX},
           {"rY", p.rY},
           {"rZ", p.rZ}};
  optional_to_json(j, "groupId", p.groupId);
}

inline void from_json(const json &j, StageMember &p) {
  required_from_json(j, "_id", p._id);
  required_from_json(j, "stageId", p.stageId);
  required_from_json(j, "userId", p.userId);
  required_from_json(j, "active", p.active);
  required_from_json(j, "isDirector", p.isDirector);
  optional_from_json(j, "groupId", p.groupId);
  // Support old stage members without name
  if(j.contains("name")) {
    required_from_json(j, "name", p.name);
  } else {
    p.name = "";
  }
  from_json(j, static_cast<VolumeProperties &>(p));
  from_json(j, static_cast<ThreeDimensionalProperties &>(p));
}

inline void to_json(json &j, const StageDevice &p) {
  j = json{{"_id", p._id},
           {"userId", p.userId},
           {"deviceId", p.deviceId},
           {"stageId", p.stageId},
           {"stageMemberId", p.stageMemberId},
           {"active", p.active},
           {"type", p.type},
           {"order", p.order},
           {"sendLocal", p.sendLocal}};
  optional_to_json(j, "groupId", p.groupId);
}

inline void from_json(const json &j, StageDevice &p) {
  required_from_json(j, "_id", p._id);
  required_from_json(j, "userId", p.userId);
  required_from_json(j, "deviceId", p.deviceId);
  required_from_json(j, "stageId", p.stageId);
  required_from_json(j, "stageMemberId", p.stageMemberId);
  required_from_json(j, "active", p.active);
  required_from_json(j, "type", p.type);
  required_from_json(j, "order", p.order);
  required_from_json(j, "sendLocal", p.sendLocal);
  optional_from_json(j, "groupId", p.groupId);
}

inline void to_json(json &j, const Channel &p) {
  j = json{{"active", p.active}};
  optional_to_json(j, "label", p.label);
}

inline void from_json(const json &j, Channel &p) {
  required_from_json(j, "active", p.active);
  optional_from_json(j, "label", p.label);
}

inline void to_json(json &j, const SoundCard &p) {
  j = json{{"_id", p._id},
           {"uuid", p.uuid},
           {"deviceId", p.deviceId},
           {"audioEngine", p.audioEngine},
           {"audioDriver", p.audioDriver},
           {"type", p.type},
           {"label", p.label},
           {"sampleRate", p.sampleRate},
           {"sampleRates", p.sampleRates},
           {"periodSize", p.periodSize},
           {"numPeriods", p.numPeriods},
           {"bufferSize", p.bufferSize},
           {"channels", p.channels},
           {"online", p.online},
           {"userId", p.userId}};
  optional_to_json(j, "isDefault", p.isDefault);
  optional_to_json(j, "softwareLatency", p.softwareLatency);
}

inline void from_json(const json &j, SoundCard &p) {
  required_from_json(j, "_id", p._id);
  required_from_json(j, "uuid", p.uuid);
  required_from_json(j, "deviceId", p.deviceId);
  required_from_json(j, "audioEngine", p.audioEngine);
  required_from_json(j, "audioDriver", p.audioDriver);
  required_from_json(j, "type", p.type);
  required_from_json(j, "label", p.label);
  required_from_json(j, "sampleRate", p.sampleRate);
  required_from_json(j, "sampleRates", p.sampleRates);
  required_from_json(j, "bufferSize", p.bufferSize);
  required_from_json(j, "periodSize", p.periodSize);
  required_from_json(j, "numPeriods", p.numPeriods);
  required_from_json(j, "channels", p.channels);
  required_from_json(j, "online", p.online);
  required_from_json(j, "userId", p.userId);
  optional_from_json(j, "isDefault", p.isDefault);
  optional_from_json(j, "softwareLatency", p.softwareLatency);
}

inline void to_json(json &j, const VideoTrack &p) {
  j = json{{"_id", p._id},
           {"stageDeviceId", p.stageDeviceId},
           {"stageMemberId", p.stageMemberId},
           {"stageId", p.stageId},
           {"deviceId", p.deviceId},
           {"userId", p.userId},
           {"type", p.type}};
}

inline void from_json(const json &j, VideoTrack &p) {
  required_from_json(j, "_id", p._id);
  required_from_json(j, "stageDeviceId", p.stageDeviceId);
  required_from_json(j, "stageMemberId", p.stageMemberId);
  required_from_json(j, "stageId", p.stageId);
  required_from_json(j, "deviceId", p.deviceId);
  required_from_json(j, "userId", p.userId);
  required_from_json(j, "type", p.type);
}

inline void to_json(json &j, const AudioTrack &p) {
  j = json{{"_id", p._id},
           {"stageDeviceId", p.stageDeviceId},
           {"stageMemberId", p.stageMemberId},
           {"stageId", p.stageId},
           {"deviceId", p.deviceId},
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
  optional_to_json(j, "uuid", p.uuid);
  optional_to_json(j, "name", p.name);
  optional_to_json(j, "sourceChannel", p.sourceChannel);
  optional_to_json(j, "ovSourcePort", p.ovSourcePort);
}

inline void from_json(const json &j, AudioTrack &p) {
  required_from_json(j, "_id", p._id);
  required_from_json(j, "stageDeviceId", p.stageDeviceId);
  required_from_json(j, "stageMemberId", p.stageMemberId);
  required_from_json(j, "stageId", p.stageId);
  required_from_json(j, "deviceId", p.deviceId);
  required_from_json(j, "userId", p.userId);
  required_from_json(j, "type", p.type);
  optional_from_json(j, "name", p.name);
  optional_from_json(j, "sourceChannel", p.sourceChannel);
  optional_from_json(j, "ovSourcePort", p.ovSourcePort);
  optional_from_json(j, "uuid", p.uuid);
  from_json(j, static_cast<VolumeProperties &>(p));
  from_json(j, static_cast<ThreeDimensionalProperties &>(p));
}

inline void to_json(json &j, const User &p) {
  j = json{{"_id", p._id},
           {"uid", p.uid},
           {"name", p.name},
           {"canCreateStage", p.canCreateStage}};
  optional_to_json(j, "avatarUrl", p.avatarUrl);
  optional_to_json(j, "stageId", p.stageId);
  optional_to_json(j, "groupId", p.groupId);
  optional_to_json(j, "stageMemberId", p.stageMemberId);
}

inline void from_json(const json &j, User &p) {
  required_from_json(j, "_id", p._id);
  required_from_json(j, "uid", p.uid);
  required_from_json(j, "name", p.name);
  required_from_json(j, "canCreateStage", p.canCreateStage);
  optional_from_json(j, "avatarUrl", p.avatarUrl);
  optional_from_json(j, "stageId", p.stageId);
  optional_from_json(j, "groupId", p.groupId);
  optional_from_json(j, "stageMemberId", p.stageMemberId);
}

inline void to_json(json &j, const WholeStage &p) {
  j = json{{"users", p.users},
           {"devices", p.devices},
           {"soundCards", p.soundCards},
           {"stages", p.stages},
           {"groups", p.groups},
           {"groups", p.customGroups},
           {"stageMembers", p.stageMembers},
           {"stageDevices", p.stageDevices},
           {"audioTracks", p.audioTracks},
           {"videoTracks", p.videoTracks}};
}

inline void from_json(const json &j, WholeStage &p) {
  required_from_json(j, "users", p.users);
  required_from_json(j, "devices", p.devices);
  required_from_json(j, "soundCards", p.soundCards);
  required_from_json(j, "stages", p.stages);
  required_from_json(j, "groups", p.groups);
  required_from_json(j, "customGroups", p.customGroups);
  required_from_json(j, "stageMembers", p.stageMembers);
  required_from_json(j, "stageDevices", p.stageDevices);
  required_from_json(j, "audioTracks", p.audioTracks);
  required_from_json(j, "videoTracks", p.videoTracks);
}

inline void to_json(json &j, const P2PRestart &p) {
  j = json{
      {"from", p.from},
      {"to", p.to}};
}
inline void from_json(const json &j, P2PRestart &p) {
  required_from_json(j, "from", p.from);
  required_from_json(j, "to", p.to);
}
inline void to_json(json &j, const SessionDescriptionInit &p) {
  j = json{
      {"sdp", p.sdp},
      {"type", p.type}};
}
inline void from_json(const json &j, SessionDescriptionInit &p) {
  required_from_json(j, "sdp", p.sdp);
  required_from_json(j, "type", p.type);
}
inline void to_json(json &j, const P2POffer &p) {
  j = json{
      {"from", p.from},
      {"to", p.to},
      {"offer", p.offer}};
}
inline void from_json(const json &j, P2POffer &p) {
  required_from_json(j, "from", p.from);
  required_from_json(j, "to", p.to);
  required_from_json(j, "offer", p.offer);
}
inline void to_json(json &j, const P2PAnswer &p) {
  j = json{
      {"from", p.from},
      {"to", p.to},
      {"answer", p.answer}};
}
inline void from_json(const json &j, P2PAnswer &p) {
  required_from_json(j, "from", p.from);
  required_from_json(j, "to", p.to);
  required_from_json(j, "answer", p.answer);
}
inline void to_json(json &j, const IceCandidateInit &p) {
  j = json{
      {"candidate", p.candidate},
      {"sdpMLineIndex", p.sdpMLineIndex},
      {"sdpMid", p.sdpMid},
  };
}
inline void from_json(const json &j, IceCandidateInit &p) {
  required_from_json(j, "candidate", p.candidate);
  required_from_json(j, "sdpMLineIndex", p.sdpMLineIndex);
  required_from_json(j, "sdpMid", p.sdpMid);
}
inline void to_json(json &j, const IceCandidate &p) {
  j = json{
      {"from", p.from},
      {"to", p.to},
  };
  optional_to_json(j, "iceCandidate", p.iceCandidate);
}
inline void from_json(const json &j, IceCandidate &p) {
  required_from_json(j, "from", p.from);
  required_from_json(j, "to", p.to);
  optional_from_json(j, "iceCandidate", p.iceCandidate);
}

} // namespace DigitalStage::Types

#endif