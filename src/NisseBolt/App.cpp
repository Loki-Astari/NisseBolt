#include "App.h"
#include <type_traits>
#include "Ack.h"
#include "Say.h"
#include "ThorsSlack/SlackEventHandler.h"
#include "ThorsSlack/SlackClient.h"


using namespace ThorsAnvil::Nisse::Bolt;

template<typename T>
concept HasStringChannel = requires(T t) {{ t.channel } -> std::convertible_to<std::string const&>;};
template<typename T>
concept HasOptStringTS   = requires(T t) {{ t.ts } -> std::convertible_to<std::optional<std::string> const&>;};

static const std::string emptyString;

template<typename T>
std::string const& getChannel(T const&)                                 {return emptyString;}
THORSSLACK_HEADER_ONLY_INCLUDE
std::string const& getChannel(HasStringChannel auto const& event)       {return event.channel;}

template<typename T>
std::optional<std::string> getTS(T const&)                              {return {};}
THORSSLACK_HEADER_ONLY_INCLUDE
std::optional<std::string> getTS(HasOptStringTS auto const& event)      {return event.ts;}

template<typename T>
void App::handleEvent(ThorsAnvil::Slack::EventRequest<T> const& request)
{
    Say     say{client, Where{.channel = getChannel(request.event), .ts = getTS(request.event)}};
    for (AnyEventRunner const& anyRunner: eventRunners) {
        EventRunner<T> const& eventRunner = std::get<EventRunner<T>>(anyRunner);
        eventRunner(request.event, say);
    }
}

THORSSLACK_HEADER_ONLY_INCLUDE
App::App(AppConfig const& config, std::string const& slot)
    : slot{slot}
    , client(config.botToken, config.userToken)
    , slackHandler(client, config.signingSecret, eventHandlerMap, slashCommandHandlerMap, actionHandlerMap, viewHandlerMap)
{
    addEventHandlers();
    addSlashCommandHandlers();
    addUserActionHandlers();
}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::addSlashCommandHandlers()  {}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::addUserActionHandlers()    {}

template<typename T>
void App::addEventHandler()
{
    eventHandlerMap[T::typeName()]  = [&](ThorsAnvil::Slack::EventRequest<T> const& request)    {handleEvent(request);};
}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::addEventHandlers()
{
    eventHandlerMap[Event::Message::typeName()]                     = [&](ThorsAnvil::Slack::EventRequest<Event::Message> const& request)                      {handleEventMessage(request);};

    // This calls addEventHandler() for each Slash::Event::<Type> in AnyEventRunner (see Runners.h)
    addAllEventHandlers();
}

THORSSLACK_HEADER_ONLY_INCLUDE
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
        },
        ThorsAnvil::ThorsMug::Action
        {
                ThorsAnvil::Nisse::HTTP::Method::POST,
                slot + "/slash/{CommandString}",
                [&](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response){slackHandler.handleSlashCommand(request, response);return true;},
                [&](ThorsAnvil::Nisse::HTTP::Request const& request){return slackHandler.validateRequest(request);}
        },
        ThorsAnvil::ThorsMug::Action
        {
                ThorsAnvil::Nisse::HTTP::Method::POST,
                slot + "/useraction",
                [&](ThorsAnvil::Nisse::HTTP::Request const& request, ThorsAnvil::Nisse::HTTP::Response& response){slackHandler.handleUserActions(request, response);return true;},
                [&](ThorsAnvil::Nisse::HTTP::Request const& request){return slackHandler.validateRequest(request);}
        }
    };
}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::message(Filter filter, MessageRunner runner)
{
    messageRunners.emplace_back(std::move(filter), std::move(runner));
}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::message(std::string filter, MessageRunner runner)
{
    message([filter = std::move(filter)](ThorsAnvil::Slack::Event::Message const& message){return message.text.find(filter) != std::string::npos;}, std::move(runner));
}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::message(std::regex filter, MessageRunner runner)
{
    message([filter = std::move(filter)](ThorsAnvil::Slack::Event::Message const& message){return std::regex_search(message.text, filter);}, std::move(runner));
}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::message(MessageRunner runner)
{
    message([](ThorsAnvil::Slack::Event::Message const&){return true;}, std::move(runner));
}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::handleEventMessage(ThorsAnvil::Slack::EventRequest<ThorsAnvil::Slack::Event::Message> const& request)
{
    Say     say{client, Where{.channel = request.event.channel.value_or("bad"), .ts = request.event.ts}};
    for (auto const& messageRunner: messageRunners) {
        if (messageRunner.first(request.event)) {
            messageRunner.second(request.event, say);
        }
    }
}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::command(std::string const& command, SlashCommandRunner runner)
{
    std::string index = (command.size() != 0 && command[0] == '/') ? command : (std::string("/") + command);

    slashCommandHandlerMap.insert_or_assign(index,
                                            [run = std::move(runner)](ThorsAnvil::Slack::SlashCommandRequest const& request)
                                            {
                                                Ack         ack{request.response};
                                                Response    response;
                                                run(ack, response, request.command);
                                            }
    );
}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::action(std::string const& actionId, ActionRunner runner)
{
    actionHandlerMap.insert_or_assign(actionId,
      ThorsAnvil::Slack::FilterHandler{"",
                                        [run = std::move(runner)](ThorsAnvil::Slack::ActionHandlerRequest const& request)
                                        {
                                                Ack         ack{request.response};
                                                Response    response;
                                                run(ack, response, request.command, request.value);
                                        }
                                      }
    );
}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::action(std::string const& actionId, std::string const& blockId, ActionRunner runner)
{
    actionHandlerMap.insert_or_assign(actionId,
      ThorsAnvil::Slack::FilterHandler{blockId,
                                       [run = std::move(runner)](ThorsAnvil::Slack::ActionHandlerRequest const& request)
                                        {
                                                Ack         ack{request.response};
                                                Response    response;
                                                run(ack, response, request.command, request.value);
                                        }
                                      }
    );
}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::viewOpen(std::string const& triggerId, View const& view)
{
    getClient().sendMessage(ThorsAnvil::Slack::API::Views::Open{view.getDisplayView(), triggerId, {}},
                            [&](ThorsAnvil::Slack::API::Views::ViewReply reply)
                            {
                                viewHandlerMap.insert_or_assign(reply.view.id, view.handlers);
                            }
                           );
}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::viewPush(std::string const& triggerId, View const& view)
{
    getClient().sendMessage(ThorsAnvil::Slack::API::Views::Push{view.getDisplayView(), triggerId, {}},
                            [&](ThorsAnvil::Slack::API::Views::ViewReply reply)
                            {
                                auto insert = viewHandlerMap.insert_or_assign(reply.view.id, view.handlers);
                                if (reply.view.previous_view_id) {
                                    insert.first->second.parentView = (*reply.view.previous_view_id);
                                }
                            }
                           );
}

THORSSLACK_HEADER_ONLY_INCLUDE
void App::viewUpdate(std::string const& viewId, ThorsAnvil::Slack::API::Views::View display)
{
    getClient().sendMessage(ThorsAnvil::Slack::API::Views::Update{std::move(display), viewId, {}, {}});
}
