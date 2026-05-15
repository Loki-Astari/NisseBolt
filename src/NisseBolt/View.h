#ifndef  THORSANVIL_NISSE_BOLT_VIEW_H
#define  THORSANVIL_NISSE_BOLT_VIEW_H


#include "NisseBoltConfig.h"
#include "Handlers.h"
#include "ThorsSlack/APIViews.h"

namespace ThorsAnvil::Nisse::Bolt
{

class App;
class View
{
    friend class App;
    ThorsAnvil::Slack::API::Views::View             view;
    ThorsAnvil::Slack::View                         viewHandlers;

    public:
        View(ThorsAnvil::Slack::API::Views::View&& view, ViewSubmitHandler&& submitHandler);
        View(ThorsAnvil::Slack::API::Views::View&& view, ViewSubmitHandler&& submitHandler, ViewClosedHandler&& closeHandler);

        // Handle User Actions.
        void action(std::string const& actionId, ActionHandler&& handler);
};


}
#endif
