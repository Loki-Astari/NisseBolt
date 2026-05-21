#include "View.h"

using namespace ThorsAnvil::Nisse::Bolt;

THORSSLACK_HEADER_ONLY_INCLUDE
View::View(ThorsAnvil::Slack::API::Views::View display, ViewSubmitRunner submitRunner, ViewClosedRunner closeRunner)
    : display(std::move(display))
    , handlers{
                    [submit = std::move(submitRunner)](ThorsAnvil::Nisse::HTTP::Request const& /*request*/, ThorsAnvil::Nisse::HTTP::Response& response, ThorsAnvil::Slack::API::Views::ViewSubmission const& view)
                    {
                        Ack         ack{response};
                        Response    response1;
                        submit(ack, response1, view);
                    },
                    [close = std::move(closeRunner)](ThorsAnvil::Nisse::HTTP::Request const& /*request*/, ThorsAnvil::Nisse::HTTP::Response& response, ThorsAnvil::Slack::API::Views::ViewClosed const& view)
                    {
                        Ack         ack{response};
                        Response    response1;
                        close(ack, response1, view);
                    },
                    {},
                    ""
              }
{
    display.notify_on_close = true;
}

THORSSLACK_HEADER_ONLY_INCLUDE
View::View(ThorsAnvil::Slack::API::Views::View display, ViewSubmitRunner submitRunner)
    : display(std::move(display))
    , handlers{
                    [submit = std::move(submitRunner)](ThorsAnvil::Nisse::HTTP::Request const& /*request*/, ThorsAnvil::Nisse::HTTP::Response& response, ThorsAnvil::Slack::API::Views::ViewSubmission const& view)
                    {
                        Ack         ack{response};
                        Response    response1;
                        submit(ack, response1, view);
                    },
                    [](ThorsAnvil::Nisse::HTTP::Request const& /*request*/, ThorsAnvil::Nisse::HTTP::Response& /*response*/, ThorsAnvil::Slack::API::Views::ViewClosed const& /*view*/)  {},
                    {},
                    ""
              }
{
    display.notify_on_close = true;
}

THORSSLACK_HEADER_ONLY_INCLUDE
void View::action(std::string const& actionId, ActionRunner runner)
{
    handlers.actionHandlerMap.insert_or_assign(actionId,
      ThorsAnvil::Slack::FilterHandler{"",
                                       [h = std::move(runner)](ThorsAnvil::Slack::ActionHandlerRequest const& request)
                                        {
                                                Ack         ack{request.response};
                                                Response    response;
                                                h(ack, response, request.command, request.value);
                                        }
                                      }
    );
}

THORSSLACK_HEADER_ONLY_INCLUDE
void View::action(std::string const& actionId, std::string const& blockId, ActionRunner runner)
{
    handlers.actionHandlerMap.insert_or_assign(actionId,
      ThorsAnvil::Slack::FilterHandler{blockId,
                                       [h = std::move(runner)](ThorsAnvil::Slack::ActionHandlerRequest const& request)
                                        {
                                                Ack         ack{request.response};
                                                Response    response;
                                                h(ack, response, request.command, request.value);
                                        }
                                      }
    );
}

THORSSLACK_HEADER_ONLY_INCLUDE
ThorsAnvil::Slack::API::Views::View const& View::getDisplayView() const
{
    return display;
}
