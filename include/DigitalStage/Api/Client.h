#ifndef DS_CLIENT
#define DS_CLIENT

#include "Events.h"
#include "Store.h"
#include <future>
#include <mutex>
#include <optional>
#include <sigslot/signal.hpp>
#include <teckos/client.h>

namespace DigitalStage {
    namespace Api {

        using namespace DigitalStage::Types;

        class InvalidPayloadException : public std::runtime_error {
        public:
            InvalidPayloadException(const std::string& what = "") : std::runtime_error(what) {}
        };

        class Client {
        public:
            /**
             * You can use this empty struct as a type for generating a life-cycle token.
             * @see https://github.com/palacaze/sigslot#automatic-slot-lifetime-tracking
             */
            struct Token {
            };

            explicit Client(std::string const& apiUrl);
            ~Client();

            void connect(const std::string& apiToken, const nlohmann::json& initialDevicePayload = nullptr);

            void disconnect();

            std::weak_ptr<Store> getStore() const;

            bool isConnected();

            void send(const std::string& event, const nlohmann::json& message) noexcept(false);

            void send(const std::string& event, const nlohmann::json& message, teckos::Callback callback) noexcept(false);

            sigslot::signal<bool /* expected */> disconnected;
            sigslot::signal<std::weak_ptr<DigitalStage::Api::Store>> ready;
            sigslot::signal<const Device, std::weak_ptr<DigitalStage::Api::Store>> localDeviceReady;
            sigslot::signal<const User, std::weak_ptr<DigitalStage::Api::Store>> localUserReady;

            sigslot::signal<const ID_TYPE&, const std::optional<ID_TYPE>&, std::weak_ptr<DigitalStage::Api::Store>> stageJoined;
            sigslot::signal<std::weak_ptr<DigitalStage::Api::Store>> stageLeft;

            sigslot::signal<const Device, std::weak_ptr<DigitalStage::Api::Store>> deviceAdded;
            sigslot::signal<const ID_TYPE&, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store>> deviceChanged;
            sigslot::signal<const ID_TYPE&, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store>> localDeviceChanged;
            /**
             * This will send when the audio driver has been changed.
             * The first parameter contains the new audio driver as optional value
             */
            sigslot::signal<std::optional<std::string>, std::weak_ptr<DigitalStage::Api::Store>> audioDriverSelected;
            /**
             * This will send when another input sound card has been selected.
             * First parameter is the ID of the sound card as optional value
             */
            sigslot::signal<const std::optional<ID_TYPE>&, std::weak_ptr<DigitalStage::Api::Store>> inputSoundCardSelected;
            /**
             * This will send when another output sound card has been selected.
             * First parameter is the ID of the sound card as optional value
             */
            sigslot::signal<const std::optional<ID_TYPE>&, std::weak_ptr<DigitalStage::Api::Store>> outputSoundCardSelected;
            sigslot::signal<const ID_TYPE&, std::weak_ptr<DigitalStage::Api::Store>> deviceRemoved;

            sigslot::signal<const Stage, std::weak_ptr<DigitalStage::Api::Store>> stageAdded;
            sigslot::signal<const ID_TYPE&, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store>> stageChanged;
            sigslot::signal<const ID_TYPE&, std::weak_ptr<DigitalStage::Api::Store>> stageRemoved;

            sigslot::signal<const Group, std::weak_ptr<DigitalStage::Api::Store>> groupAdded;
            sigslot::signal<const ID_TYPE&, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store>> groupChanged;
            sigslot::signal<const ID_TYPE&, std::weak_ptr<DigitalStage::Api::Store>> groupRemoved;

            sigslot::signal<const CustomGroup, std::weak_ptr<DigitalStage::Api::Store>> customGroupAdded;
            sigslot::signal<const ID_TYPE&, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store>> customGroupChanged;
            sigslot::signal<const CustomGroup, std::weak_ptr<DigitalStage::Api::Store>> customGroupRemoved;

            sigslot::signal<const StageMember, std::weak_ptr<DigitalStage::Api::Store>> stageMemberAdded;
            sigslot::signal<const ID_TYPE&, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store>> stageMemberChanged;
            sigslot::signal<const ID_TYPE&, std::weak_ptr<DigitalStage::Api::Store>> stageMemberRemoved;

            sigslot::signal<const StageDevice, std::weak_ptr<DigitalStage::Api::Store>> stageDeviceAdded;
            sigslot::signal<const ID_TYPE&, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store>> stageDeviceChanged;
            sigslot::signal<const StageDevice&, std::weak_ptr<DigitalStage::Api::Store>> stageDeviceRemoved;

            sigslot::signal<const SoundCard, std::weak_ptr<DigitalStage::Api::Store>> soundCardAdded;
            sigslot::signal<const ID_TYPE&, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store>> soundCardChanged;
            sigslot::signal<const ID_TYPE&, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store>> inputSoundCardChanged;
            sigslot::signal<const ID_TYPE&, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store>> outputSoundCardChanged;
            sigslot::signal<const ID_TYPE&, std::weak_ptr<DigitalStage::Api::Store>> soundCardRemoved;

            sigslot::signal<const VideoTrack, std::weak_ptr<DigitalStage::Api::Store>> videoTrackAdded;
            sigslot::signal<const ID_TYPE&, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store>> videoTrackChanged;
            sigslot::signal<const VideoTrack, std::weak_ptr<DigitalStage::Api::Store>> videoTrackRemoved;

            sigslot::signal<const AudioTrack, std::weak_ptr<DigitalStage::Api::Store>> audioTrackAdded;
            sigslot::signal<const ID_TYPE&, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store>> audioTrackChanged;
            sigslot::signal<const AudioTrack, std::weak_ptr<DigitalStage::Api::Store>> audioTrackRemoved;

            sigslot::signal<const User, std::weak_ptr<DigitalStage::Api::Store>> userAdded;
            sigslot::signal<const ID_TYPE&, nlohmann::json, std::weak_ptr<DigitalStage::Api::Store>> userChanged;
            sigslot::signal<const ID_TYPE&, std::weak_ptr<DigitalStage::Api::Store>> userRemoved;

            sigslot::signal<const P2PRestart, std::weak_ptr<DigitalStage::Api::Store>> p2pRestart;
            sigslot::signal<const P2PAnswer, std::weak_ptr<DigitalStage::Api::Store>> p2pAnswer;
            sigslot::signal<const P2POffer, std::weak_ptr<DigitalStage::Api::Store>> p2pOffer;
            sigslot::signal<const IceCandidate, std::weak_ptr<DigitalStage::Api::Store>> iceCandidate;

            sigslot::signal<const std::exception&> error;

            /**
             * Decodes an invitation key.
             * Returns nothing, if invitation key is invalid or expired.
             * @return pair of stage ID and optional group ID
             */
            std::future<std::pair<std::string, std::optional<std::string>>> decodeInvitationCode(const std::string& code);

            /**
             * Revoke an invitation code and return a new one.
             * This invalidates the old code.
             * @param stage ID
             * @param group ID
             */
            std::future<std::string> revokeInvitationCode(const std::string& stageId, const std::optional<std::string>& groupId = std::nullopt);

            std::future<std::string> encodeInvitationCode(const std::string& stageId, const std::optional<std::string>& groupId = std::nullopt);

            std::future<bool> joinStage(std::string stageId, std::optional<std::string> const & groupId = {}, std::optional<std::string> const & password = {});
            std::future<bool> leaveStage();

            // Suboptimal to put this here, but it is tested directly
            void handleMessage(const std::string& event, const nlohmann::json& payload);

        private:
            const std::string apiUrl_;
            std::shared_ptr<Store> store_;
            std::unique_ptr<teckos::client> wsclient_;
        };
    } // namespace Api
} // namespace DigitalStage

#endif // DS_CLIENT
