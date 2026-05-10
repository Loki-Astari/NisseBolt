#ifndef  THORSANVIL_NISSE_BOLT_APP_H
#define  THORSANVIL_NISSE_BOLT_APP_H

#include "NisseBoltConfig.h"
#include "AppConfig.h"
#include "Say.h"
#include "Ack.h"

#include "ThorsMug/MugPlugin.h"
#include "ThorsSlack/SlackClient.h"
#include "ThorsSlack/SlackEventHandler.h"

#include <map>
#include <string>
#include <vector>
#include <functional>
#include <utility>
#include <memory>
#include <regex>

namespace ThorsAnvil::Nisse::Bolt
{
namespace Event = ThorsAnvil::Slack::Event;

template<typename T>
using EventHandler          = std::function<void(T const& event, Say const& say)>;
using AnyEventHandler       = std::variant<
                                        EventHandler<Event::AppDeleted>, EventHandler<Event::AppHomeOpened>, EventHandler<Event::AppInstalled>, EventHandler<Event::AppRateLimited>, EventHandler<Event::AppRequested>, EventHandler<Event::AppUninstalledTeam>, EventHandler<Event::AppUninstalled>, EventHandler<Event::AppMentioned>,
                                        EventHandler<Event::AssistantThreadContextChanged>, EventHandler<Event::AssistantThreadStarted>,
                                        EventHandler<Event::CallRejected>,
                                        EventHandler<Event::ChannelArchive>, EventHandler<Event::ChannelCreated>, EventHandler<Event::ChannelDeleted>, EventHandler<Event::ChannelHistoryChanged>, EventHandler<Event::ChannelIdChanged>, EventHandler<Event::ChannelLeft>, EventHandler<Event::ChannelPostingPermissions>, EventHandler<Event::ChannelRename>, EventHandler<Event::ChannelShared>, EventHandler<Event::ChannelUnshared>,
                                        EventHandler<Event::DndUpdated>, EventHandler<Event::DndUpdatedUser>,
                                        EventHandler<Event::EmailDomainChanged>,
                                        EventHandler<Event::EmojiChanged>,
                                        EventHandler<Event::EntityDetailsRequested>,
                                        EventHandler<Event::FileChange>, EventHandler<Event::FileCommentAdded>, EventHandler<Event::FileCommentDeleted>, EventHandler<Event::FileCommentEdited>, EventHandler<Event::FileCreated>, EventHandler<Event::FileDeleted>, EventHandler<Event::FilePublic>, EventHandler<Event::FileShared>, EventHandler<Event::FileUnshared>,
                                        EventHandler<Event::FunctionExecuted>,
                                        EventHandler<Event::GridMigrationFinished>, EventHandler<Event::GridMigrationStarted>,
                                        EventHandler<Event::GroupClose>, EventHandler<Event::GroupDeleted>, EventHandler<Event::GroupHistoryChanged>, EventHandler<Event::GroupLeft>, EventHandler<Event::GroupOpen>, EventHandler<Event::GroupRename>,
                                        EventHandler<Event::ImClose>, EventHandler<Event::ImCreated>, EventHandler<Event::ImHistoryChanged>, EventHandler<Event::ImOpen>,
                                        EventHandler<Event::InviteRequested>,
                                        EventHandler<Event::LinkShared>,
                                        EventHandler<Event::MemberJoinedChannel>, EventHandler<Event::MemberLeftChannel>,
                                        // EventHandler<Event::Message>,
                                        EventHandler<Event::MessageMetadataPosted>, EventHandler<Event::MessageMetadataUpdated>, EventHandler<Event::MessageMetadataDeleted>,
                                        EventHandler<Event::PinAdded>, EventHandler<Event::PinRemoved>,
                                        EventHandler<Event::ReactionAdded>, EventHandler<Event::ReactionRemoved>,
                                        EventHandler<Event::SharedChannelInviteAccepted>, EventHandler<Event::SharedChannelInviteApproved>, EventHandler<Event::SharedChannelInviteDeclined>, EventHandler<Event::SharedChannelInviteReceived>, EventHandler<Event::SharedChannelInviteRequested>,
                                        EventHandler<Event::StarAdded>, EventHandler<Event::StarRemoved>,
                                        EventHandler<Event::SubteamCreated>, EventHandler<Event::SubteamMembersChanged>, EventHandler<Event::SubteamSelfAdded>, EventHandler<Event::SubteamSelfRemoved>, EventHandler<Event::SubteamUpdated>,
                                        EventHandler<Event::TeamAccessGranted>, EventHandler<Event::TeamAccessRevoked>, EventHandler<Event::TeamDomainChange>, EventHandler<Event::TeamJoin>, EventHandler<Event::TeamRename>,
                                        EventHandler<Event::TokensRevoked>,
                                        EventHandler<Event::UserChange>, EventHandler<Event::UserConnection>, EventHandler<Event::UserHuddleChanged>
                              >;

using Filter                = std::function<bool(ThorsAnvil::Slack::Event::Message const&)>;
using MessageHandler        = EventHandler<ThorsAnvil::Slack::Event::Message>;

struct Response {};

using SlashCommandHandler   = std::function<void(Ack const&, Response const&, ThorsAnvil::Slack::SlashCommand const&)>;

class App: ThorsAnvil::ThorsMug::MugPluginSimple
{
    std::string                                     slot;
    ThorsAnvil::Slack::CmdMap                       cmdMap;
    ThorsAnvil::Slack::EventHandlerMap              eventHandlerMap;
    ThorsAnvil::Slack::SlackClient                  client;
    ThorsAnvil::Slack::SlackEventHandler            slackHandler;
    ThorsAnvil::Slack::SlashCommandHandlerMap       slashCommandHandlerMap;
    std::vector<std::pair<Filter, MessageHandler>>  messageHandlers;
    std::vector<AnyEventHandler>                    eventHandlers;

    public:
        App(AppConfig const& config);
        virtual std::vector<ThorsAnvil::ThorsMug::Action> getAction() override;

        // register handlers for the standard Message events that come from the slack server.
        void message(Filter&& filter, MessageHandler&& handler);            // Generic filter.
        void message(std::string filter, MessageHandler&& handler);         // Simple to use string filter.
        void message(std::regex filter, MessageHandler&& handler);          // Simplish to use regular expression filter
        void message(MessageHandler&& handler);                             // No filter handle all events.

        // register handlers for other Events (other than Message) that come from the slack server.
        void event(EventHandler<Event::Message>&& handler)                  {message(std::move(handler));}
        template<typename E>
        void event(EventHandler<E>&& handler)                               {eventHandlers.emplace_back(std::move(handler));}

        // Handle slash commands:
        void command(std::string const& command, SlashCommandHandler&& handler);

    private:
        // Handle incoming events and send to the registered handlers.
        template<typename T>
        void handleEvent(ThorsAnvil::Slack::EventRequest<T> const& request);
        void handleEventMessage(ThorsAnvil::Slack::EventRequest<ThorsAnvil::Slack::Event::Message> const& request);


        // Friend declaration so mugInterface can register robots.
        template<typename Server, typename ServerConfig>
        friend ThorsAnvil::ThorsMug::MugPlugin* mugCreateBoltInstance(int init, char const* configStr);

        // Addig and removing Bots at runtime.
        template<typename Server, typename ServerConfig>
        static ThorsAnvil::ThorsMug::MugPlugin* add(ServerConfig&& serverConfig);
        template<typename Server, typename ServerConfig>
        static ThorsAnvil::ThorsMug::MugPlugin* rem(ServerConfig&& serverConfig);
    private:
        // Currently active Bots.
        static std::map<std::string, std::unique_ptr<App>>& getServerInfo();
};


/**
 * Template functions that need to be in header file
 */
template<typename Server, typename ServerConfig>
ThorsAnvil::ThorsMug::MugPlugin* App::add(ServerConfig&& serverConfig)
{
    std::map<std::string, std::unique_ptr<App>>&    servers = getServerInfo();
    std::string const& slot = static_cast<AppConfig&>(serverConfig).slot;
    auto find = servers.find(slot);
    if (find != servers.end() && find->second.get() != nullptr) {
        // Extracting the pointer here
        // So I can use it in typeid() without generating a warning message.
        App* rawApp = find->second.get();
        ThorsLogAndThrowError(std::runtime_error, "ThorsAnvil::Nisse::Bolt::App", "add", "Can not load Mug Server this slot >", slot, " is already being used. Current: ", typeid(*rawApp).name(), " New: ", typeid(Server).name());
    }
    find->second = std::make_unique<Server>(std::move(serverConfig));
    return find->second.get();
}

template<typename Server, typename ServerConfig>
ThorsAnvil::ThorsMug::MugPlugin* App::rem(ServerConfig&& serverConfig)
{
    std::map<std::string, std::unique_ptr<App>>&    servers = getServerInfo();
    auto find = servers.find(static_cast<AppConfig&>(serverConfig).slot);
    if (find != servers.end()) {
        find->second.reset();
    }
    return nullptr;
}

template<typename Server, typename ServerConfig>
ThorsAnvil::ThorsMug::MugPlugin* mugCreateBoltInstance(int init, char const* configStr)
{
    ServerConfig    config = ThorsAnvil::Serialize::jsonBuilder<ServerConfig>(std::stringstream{configStr});
    if (init) {
        return ThorsAnvil::Nisse::Bolt::App::add<Server>(config);
    }
    else {
        return ThorsAnvil::Nisse::Bolt::App::rem<Server>(config);
    }
}

}

/**
 * Simplify the Bot interface for Mug server
 **/
#define THORS_ANVIL_NISSE_BOLT_SERVER_INIT(Config, Server)                                      \
                                                                                                \
extern "C" ThorsAnvil::ThorsMug::MugPlugin* mugCreateInstance(int init, char const* configStr)  \
{                                                                                               \
    return ThorsAnvil::Nisse::Bolt::mugCreateBoltInstance<Server, Config>(init, configStr);     \
}

#endif
