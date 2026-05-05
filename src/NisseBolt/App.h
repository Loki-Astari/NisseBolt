#ifndef  THORSANVIL_NISSE_BOLT_APP_H
#define  THORSANVIL_NISSE_BOLT_APP_H

#include "NisseBoltConfig.h"
#include "AppConfig.h"
#include "Say.h"

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

using Filter         = std::function<bool(ThorsAnvil::Slack::Event::Message const&)>;
using MessageHandler = std::function<void(ThorsAnvil::Slack::Event::Message const&, Say const& say)>;

class App: ThorsAnvil::ThorsMug::MugPluginSimple
{
    std::string                             slot;
    ThorsAnvil::Slack::CmdMap               cmdMap;
    ThorsAnvil::Slack::EventHandlerMap      eventHandlerMap;
    ThorsAnvil::Slack::SlackClient          client;
    ThorsAnvil::Slack::SlackEventHandler    slackHandler;
    std::vector<std::pair<Filter, MessageHandler>> messageHandlers;

    public:
        App(AppConfig const& config);
        virtual std::vector<ThorsAnvil::ThorsMug::Action> getAction() override;

        // register handlers for the standard Message events that come from the slack server.
        void message(Filter&& filter, MessageHandler&& handler);            // Generic filter.
        void message(std::string filter, MessageHandler&& handler);         // Simple to use string filter.
        void message(std::regex filter, MessageHandler&& handler);          // Simplish to use regular expression filter
        void message(MessageHandler&& handler);                             // No filter handle all events.

    private:
        // Handle incoming events and send to the registered handlers.
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
