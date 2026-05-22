#ifndef  THORSANVIL_NISSE_BOLT_VIEW_H
#define  THORSANVIL_NISSE_BOLT_VIEW_H


#include "NisseBoltConfig.h"
#include "Runners.h"
#include "ThorsSlack/APIViews.h"

namespace ThorsAnvil::Nisse::Bolt
{

class App;
class View
{
    friend class App;
        ThorsAnvil::Slack::API::Views::View     display;
        ThorsAnvil::Slack::View                 handlers;

    public:
        View(ThorsAnvil::Slack::API::Views::View display, ViewSubmitRunner submitRunner, ThorsAnvil::Slack::ActionHandlerMap actionHandlers = {});
        View(ThorsAnvil::Slack::API::Views::View display, ViewSubmitRunner submitRunner, ViewClosedRunner closeRunner, ThorsAnvil::Slack::ActionHandlerMap actionHandlers = {});
        virtual ~View() {}

        // Handle User Actions.
        void action(std::string const& actionId, ActionRunner runner);
        void action(std::string const& actionId, std::string const& blockId, ActionRunner runner);

        ThorsAnvil::Slack::API::Views::View const& getDisplayView() const;
};


}
#endif
