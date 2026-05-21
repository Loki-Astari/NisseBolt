#ifndef  THORSANVIL_NISSE_BOLT_APP_H
#define  THORSANVIL_NISSE_BOLT_APP_H

#include "NisseBoltConfig.h"
#include "AppConfig.h"
#include "Say.h"
#include "Ack.h"
#include "Response.h"
#include "Runners.h"
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

    std::vector<std::pair<Filter, MessageRunner>>   messageRunners;
    std::vector<AnyEventRunner>                     eventRunners;

        void addSlashCommandHandlers();
        void addUserActionHandlers();
        void addEventHandlers();
        template<typename T>
        void addEventHandler();
        template<typename... Args>
        void addEachEventHandlers(std::variant<EventRunner<Args>...>*) {(addEventHandler<Args>(), ...);}
        void addAllEventHandlers()                                     {addEachEventHandlers(static_cast<AnyEventRunner*>(nullptr));}
    public:
        App(AppConfig const& config, std::string const& slot);
        virtual std::vector<ThorsAnvil::ThorsMug::Action> getAction() override;

        // register runners for the standard Message events that come from the slack server.
        void message(Filter&& filter, MessageRunner&& runner);            // Generic filter.
        void message(std::string filter, MessageRunner&& runner);         // Simple to use string filter.
        void message(std::regex filter, MessageRunner&& runner);          // Simplish to use regular expression filter
        void message(MessageRunner&& runner);                             // No filter handle all events.

        // register runners for other Events (other than Message) that come from the slack server.
        void event(EventRunner<Event::Message>&& runner)                  {message(std::move(runner));}
        template<typename E>
        void event(EventRunner<E>&& runner)                               {eventRunners.emplace_back(std::move(runner));}

        // Handle slash commands:
        void command(std::string const& command, SlashCommandRunner&& runner);

        // Handle User Actions.
        void action(std::string const& actionId, ActionRunner&& runner);
        void action(std::string const& actionId, std::string const& blockId, ActionRunner&& runner);

        // Handle Views
        void viewOpen(std::string const& triggerId, View const& view);
        void viewPush(std::string const& triggerId, View const& view);
        void viewUpdate(std::string const& viewId, ThorsAnvil::Slack::API::Views::View view);

        // Temp
        ThorsAnvil::Slack::SlackClient const& getClient() const {return client;}
    private:
        // Handle incoming events and send to the registered runners.
        template<typename T>
        void handleEvent(ThorsAnvil::Slack::EventRequest<T> const& request);
        void handleEventMessage(ThorsAnvil::Slack::EventRequest<ThorsAnvil::Slack::Event::Message> const& request);
};

}

#endif
