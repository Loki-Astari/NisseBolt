#include "View.h"

using namespace ThorsAnvil::Nisse::Bolt;

View::View(ThorsAnvil::Slack::API::Views::View&& view, ViewSubmitHandler&& submitHandler, ViewCloseHandler&& closeHandler)
    : view{std::move(view)}
    , viewHandlers{
                    [submit = std::move(submitHandler)](ThorsAnvil::Nisse::HTTP::Request const& /*request*/, ThorsAnvil::Nisse::HTTP::Response& response, ThorsAnvil::Slack::API::Views::ViewSubmission const& view)
                    {
                        Ack         ack{response};
                        Response    response1;
                        submit(ack, response1, view);
                    },
                    [close = std::move(closeHandler)](ThorsAnvil::Nisse::HTTP::Request const& /*request*/, ThorsAnvil::Nisse::HTTP::Response& response, ThorsAnvil::Slack::API::Views::ViewClose const& view)
                    {
                        Ack         ack{response};
                        Response    response1;
                        close(ack, response1, view);
                    },
                    {}
                  }
{
    this->view.notify_on_close = true;
}

View::View(ThorsAnvil::Slack::API::Views::View&& view, ViewSubmitHandler&& submitHandler)
    : view{std::move(view)}
    , viewHandlers{
                    [submit = std::move(submitHandler)](ThorsAnvil::Nisse::HTTP::Request const& /*request*/, ThorsAnvil::Nisse::HTTP::Response& response, ThorsAnvil::Slack::API::Views::ViewSubmission const& view)
                    {
                        Ack         ack{response};
                        Response    response1;
                        submit(ack, response1, view);
                    },
                    [](ThorsAnvil::Nisse::HTTP::Request const& /*request*/, ThorsAnvil::Nisse::HTTP::Response& /*response*/, ThorsAnvil::Slack::API::Views::ViewClose const& /*view*/)  {},
                    {}
                  }
{}

void View::action(std::string const& actionId, ActionHandler&& handler)
{
    viewHandlers.actionHandlerMap.insert_or_assign(actionId, [h = std::move(handler)](ThorsAnvil::Slack::ActionHandlerRequest const& request)
                  {
                    Ack         ack{request.response};
                    Response    response;
                    h(ack, response, request.command, request.value);
                  }
    );
}
