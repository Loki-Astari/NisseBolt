#ifndef  THORSANVIL_NISSE_BOLT_APP_H
#define  THORSANVIL_NISSE_BOLT_APP_H

#include "NisseBoltConfig.h"
#include "AppConfig.h"
#include "Say.h"
#include "Ack.h"
#include "Response.h"
#include "Handlers.h"
#include "View.h"

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

class App: public ThorsAnvil::ThorsMug::MugPluginSimple
{
    std::string                                     slot;
    ThorsAnvil::Slack::EventHandlerMap              eventHandlerMap;
    ThorsAnvil::Slack::SlackClient                  client;
    ThorsAnvil::Slack::SlackEventHandler            slackHandler;
    ThorsAnvil::Slack::SlashCommandHandlerMap       slashCommandHandlerMap;
    ThorsAnvil::Slack::ActionHandlerMap             actionHandlerMap;
    ThorsAnvil::Slack::ViewHandlerMap               viewHandlerMap;

    std::vector<std::pair<Filter, MessageHandler>>  messageHandlers;
    std::vector<AnyEventHandler>                    eventHandlers;

        void addSlashCommandHandlers();
        void addUserActionHandlers();
        void addEventHandlers();
    public:
        App(AppConfig const& config, std::string const& slot);
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

        // Handle User Actions.
        void action(std::string const& actionId, ActionHandler&& handler);
        void action(std::string const& actionId, std::string const& blockId, ActionHandler&& handler);

        // Handle Views
        void viewOpen(std::string const& triggerId, View const& view);
        void viewPush(std::string const& triggerId, View const& view);
        void viewUpdate(std::string const& viewId, ThorsAnvil::Slack::API::Views::View view);

        // Temp
        ThorsAnvil::Slack::SlackClient const& getClient() const {return client;}
    private:
        // Handle incoming events and send to the registered handlers.
        template<typename T>
        void handleEvent(ThorsAnvil::Slack::EventRequest<T> const& request);
        void handleEventMessage(ThorsAnvil::Slack::EventRequest<ThorsAnvil::Slack::Event::Message> const& request);
};

}

#endif
