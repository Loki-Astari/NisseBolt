#include "NBServer.h"

#include "NisseHTTP/Request.h"
#include "NisseHTTP/Response.h"

#include <filesystem>
#include <utility>

using namespace ThorsAnvil::NisseBolt;
namespace NisHttp = ThorsAnvil::Nisse::HTTP;
namespace FS      = std::filesystem;

TASock::ServerInit NBServer::getServerInit(FS::path const& certPath, int port)
{
    if (certPath.empty()) {
        return TASock::ServerInfo{port};
    }
    FS::path chain = FS::canonical(certPath) /= "fullchain.pem";
    FS::path key   = FS::canonical(certPath) /= "privkey.pem";
    TASock::CertificateInfo certificate{chain, key};
    TASock::SSLctx ctx{TASock::SSLMethodType::Server, certificate};
    return TASock::SServerInfo{port, std::move(ctx)};
}

NBServer::NBServer(NBConfig configIn)
    : NisseServer(workerCount)
    , Slack::SlackEventHandler(configIn.signingSecret)
    , config(std::move(configIn))
    , slackClient(config.botToken, "")
{
    registerRoutes();

    // port == 0 is a test-only mode: construct the server and its routes
    // without binding a listening socket. Tests drive httpHandler() directly.
    if (config.port != 0) {
        FS::path certPath = config.certRoot.empty()
                          ? FS::path{}
                          : FS::path{config.certRoot} /= config.domainName;
        listen(getServerInit(certPath, config.port), http);
    }
}

void NBServer::registerRoutes()
{
    std::string base = config.basePath;
    NisHttp::HTTPValidate validate = [this](NisHttp::Request const& req)
    {
        return validateRequest(req);
    };

    http.addPath(NisHttp::Method::POST, base + "/commands",
                 [this](NisHttp::Request const& req, NisHttp::Response& res)
                 {
                     handleSlashCommand(req, res);
                     return true;
                 },
                 NisHttp::HTTPValidate{validate});

    http.addPath(NisHttp::Method::POST, base + "/events",
                 [this](NisHttp::Request const& req, NisHttp::Response& res)
                 {
                     handleEvent(req, res);
                     return true;
                 },
                 NisHttp::HTTPValidate{validate});

    http.addPath(NisHttp::Method::POST, base + "/actions",
                 [this](NisHttp::Request const& req, NisHttp::Response& res)
                 {
                     handleUserActions(req, res);
                     return true;
                 },
                 std::move(validate));
}

void NBServer::handleSlashWithCommand(NisHttp::Request const& request,
                                       NisHttp::Response& response,
                                       Slack::SlashCommand const& command)
{
    auto it = commandHandlers.find(command.command);
    if (it == commandHandlers.end()) {
        response.setStatus(501);
        return;
    }

    Context ctx{request, response, slackClient, command.team_id, command.user_id, command.channel_id, command.response_url};

    for (auto const& mw : middleware) {
        if (!mw(ctx)) {
            Ack ack(response);   // auto-ack on middleware short-circuit
            return;
        }
    }

    Ack     ack(response);
    Respond respond(command.response_url);
    try
    {
        it->second(command, ack, respond, ctx);
    }
    catch (std::exception const& e)
    {
        // The dispatcher owns the response from here on. Stop the auto-ack so
        // it cannot clobber the 500 / onError path when the Ack destructs.
        ack.suppress();
        if (errorHandler) {
            errorHandler(e, ctx);
        }
        else {
            response.setStatus(500);
        }
    }
}

void NBServer::command(std::string name, CommandHandler handler)
{
    commandHandlers.emplace(std::move(name), std::move(handler));
}

// ---------------------------------------------------------------------------
// Stubs — filled in by later slices (events, actions, views, options).
// Listeners registered through these entry points are stored but not yet
// invoked; the demo in NB.cpp only exercises command(), and the test suite
// covers just that path for now.
// ---------------------------------------------------------------------------
void NBServer::message(std::string substring, MessageHandler handler)
{
    messageListeners.push_back({std::regex{}, std::move(handler), false, std::move(substring)});
}
void NBServer::message(std::regex pattern, MessageHandler handler)
{
    messageListeners.push_back({std::move(pattern), std::move(handler), true, {}});
}
void NBServer::action(std::string actionId, ActionHandler handler)
{
    actionHandlers.emplace(std::move(actionId), std::move(handler));
}
void NBServer::shortcut(std::string callbackId, ShortcutHandler handler)
{
    shortcutHandlers.emplace(std::move(callbackId), std::move(handler));
}
void NBServer::view(std::string callbackId, ViewHandler handler)
{
    viewHandlers.emplace(std::move(callbackId), std::move(handler));
}
void NBServer::options(std::string actionId, OptionsHandler handler)
{
    optionsHandlers.emplace(std::move(actionId), std::move(handler));
}
void NBServer::use(Middleware mw)
{
    middleware.push_back(std::move(mw));
}
void NBServer::onError(ErrorHandler handler)
{
    errorHandler = std::move(handler);
}
ThorsAnvil::Slack::SlackClient& NBServer::client()
{
    return slackClient;
}
