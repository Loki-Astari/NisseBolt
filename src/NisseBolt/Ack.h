#ifndef  THORSANVIL_NISSE_BOLT_ACK_H
#define  THORSANVIL_NISSE_BOLT_ACK_H

#include "NisseBoltConfig.h"

#include "ThorsSlack/SlackClient.h"
#include "NisseHTTP/Response.h"

namespace ThorsAnvil::Nisse::Bolt
{

class Ack
{
    ThorsAnvil::Slack::SlackClient&         client;
    ThorsAnvil::Nisse::HTTP::Response&      response;
    public:
        Ack(Slack::SlackClient& client, ThorsAnvil::Nisse::HTTP::Response& response);
        void operator()() const;
        void operator()(int responseCode) const;
};

}

#endif
