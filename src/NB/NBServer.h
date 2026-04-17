#ifndef THORSANVIL_NISSEBOLT_NBSERVER_H
#define THORSANVIL_NISSEBOLT_NBSERVER_H

#include "NBConfig.h"
#include "NBContext.h"

#include "NisseServer/NisseServer.h"
#include "NisseHTTP/HTTPHandler.h"

#include "ThorsSlack/SlackClient.h"
#include "ThorsSlack/SlackEventHandler.h"
#include "ThorsSlack/SlashCommand.h"
#include "ThorsSlack/EventCallback.h"
#include "ThorsSlack/APIBlockActions.h"
#include "ThorsSlack/APIViews.h"

#include <filesystem>
#include <functional>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

namespace ThorsAnvil::NisseBolt
{

using ThorsAnvil::Nisse::Server::NisseServer;
using ThorsAnvil::Nisse::HTTP::HTTPHandler;
using ThorsAnvil::Nisse::HTTP::Request;
using ThorsAnvil::Nisse::HTTP::Response;

// Bolt-style facade over Nisse + ThorsSlack.
//
// Routes mounted under config.basePath (default "/slack"):
//   POST /events    -> event(...) / message(...) listeners + URL verification
//   POST /commands  -> command(...) listeners
//   POST /actions   -> action(...) / shortcut(...) / view(...) listeners
//   POST /options   -> options(...) listeners
//
// NBServer publicly inherits SlackEventHandler so the Slack-protocol layer
// (request signing, payload parsing, variant dispatch) is reused rather than
// re-implemented. The inherited API is effectively internal — application
// code interacts only through the Bolt-shaped methods below.
class NBServer: public NisseServer, public Slack::SlackEventHandler
{
    static constexpr std::size_t workerCount = 4;

    NBConfig                    config;
    HTTPHandler                 http;
    Slack::SlackClient          slackClient;

    using CommandHandler = std::function<void(Slack::SlashCommand const&, Ack&, Respond&, Context&)>;
    std::unordered_map<std::string, CommandHandler> commandHandlers;

    using MessageHandler = std::function<void(Slack::Event::EventCallback const&, Ack&, Say&, Context&)>;
    struct MessageMatcher { std::regex pattern; MessageHandler handler; bool isRegex; std::string substring; };
    std::vector<MessageMatcher> messageListeners;

    using ActionHandler   = std::function<void(Slack::API::BlockActions const&, Ack&, Respond&, Context&)>;
    using ShortcutHandler = std::function<void(Slack::API::BlockActions const&, Ack&, Context&)>;
    using ViewHandler     = std::function<void(Slack::API::Views::ViewSubmission const&, Ack&, Context&)>;
    using OptionsHandler  = std::function<void(Slack::API::BlockActions const&, Ack&, Context&)>;
    std::unordered_map<std::string, ActionHandler>   actionHandlers;
    std::unordered_map<std::string, ShortcutHandler> shortcutHandlers;
    std::unordered_map<std::string, ViewHandler>     viewHandlers;
    std::unordered_map<std::string, OptionsHandler>  optionsHandlers;

    using Middleware = std::function<bool(Context&)>;
    std::vector<Middleware>     middleware;

    using ErrorHandler = std::function<void(std::exception const&, Context&)>;
    ErrorHandler                errorHandler;

    TASock::ServerInit getServerInit(std::filesystem::path const& certPath, int port);
    void registerRoutes();

    // Override SlackEventHandler hooks that we need for Bolt dispatch.
    void handleSlashWithCommand(Request const& request, Response& response, Slack::SlashCommand const& command) override;

    public:
        explicit NBServer(NBConfig config);

        // ------------------------------------------------------------------
        // Listener registration
        // ------------------------------------------------------------------

        // Generic Events API listener. EventType must be one of the typed
        // payloads in ThorsSlack/EventCallback*.h (e.g. Event::Message,
        // Event::AppMentioned). Dispatch key comes from a type trait.
        template <class EventType>
        void event(std::function<void(EventType const&, Ack&, Say&, Context&)> handler);

        // Sugar: subscribe to `message` events filtered by substring or regex.
        void message(std::string substring, MessageHandler handler);
        void message(std::regex pattern,    MessageHandler handler);

        // Slash commands. `name` includes the leading slash, e.g. "/deploy".
        void command(std::string name, CommandHandler handler);

        // Block Kit interactive actions, keyed by action_id.
        void action(std::string actionId, ActionHandler handler);

        // Global and message shortcuts, keyed by callback_id.
        void shortcut(std::string callbackId, ShortcutHandler handler);

        // Modal view_submission / view_closed, keyed by view callback_id.
        void view(std::string callbackId, ViewHandler handler);

        // External-select option requests, keyed by action_id.
        void options(std::string actionId, OptionsHandler handler);

        // ------------------------------------------------------------------
        // Cross-cutting
        // ------------------------------------------------------------------

        void use(Middleware mw);
        void onError(ErrorHandler handler);

        Slack::SlackClient& client();

        // Exposed for tests and advanced integrations: the HTTPHandler that
        // owns the four Slack routes. Drive it with a stringstream-backed
        // Request/Response to exercise dispatch without a live socket.
        HTTPHandler& httpHandler() { return http; }
};

}

#endif
