#include "App.h"
#include "Say.h"
#include "ThorsSlack/SlackEventHandler.h"


using namespace ThorsAnvil::Nisse::Bolt;

App::App(AppConfig const& config)
    : slot{config.slot}
    , client(config.botToken, config.userToken)
    , slackHandler(config.signingSecret, cmdMap, eventHandlerMap)
{
    eventHandlerMap["Event/Message"] = [&](ThorsAnvil::Slack::EventRequest<ThorsAnvil::Slack::Event::Message> const& request){handleEventMessage(request);};
}

std::vector<ThorsAnvil::ThorsMug::Action> App::getAction()
{
    return std::vector<ThorsAnvil::ThorsMug::Action>
    {
        ThorsAnvil::ThorsMug::Action
        {
                ThorsAnvil::Nisse::HTTP::Method::POST,
                slot + "/event",
                [&](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response){slackHandler.handleEvent(request, response);return true;},
                [&](ThorsAnvil::Nisse::HTTP::Request const& request){return slackHandler.validateRequest(request);}
        }
    };
}

void App::message(Filter&& filter, MessageHandler&& handler)
{
    messageHandlers.emplace_back(std::move(filter), std::move(handler));
}

void App::message(std::string filter, MessageHandler&& handler)
{
    message([filter = std::move(filter)](ThorsAnvil::Slack::Event::Message const& message){return message.text.find(filter) != std::string::npos;}, std::move(handler));
}

void App::message(std::regex filter, MessageHandler&& handler)
{
    message([filter = std::move(filter)](ThorsAnvil::Slack::Event::Message const& message){return std::regex_search(message.text, filter);}, std::move(handler));
}

void App::message(MessageHandler&& handler)
{
    message([](ThorsAnvil::Slack::Event::Message const&){return true;}, std::move(handler));
}

void App::handleEventMessage(ThorsAnvil::Slack::EventRequest<ThorsAnvil::Slack::Event::Message> const& request)
{
    Say     say{client, Where{.channel = request.event.channel.value_or("bad"), .ts = request.event.ts}};
    for (auto const& messageHandler: messageHandlers) {
        if (messageHandler.first(request.event)) {
            messageHandler.second(request.event, say);
        }
    }
}

std::map<std::string, std::unique_ptr<App>>& App::getServerInfo()
{
    static std::map<std::string, std::unique_ptr<App>>  apps;
    return apps;
}
