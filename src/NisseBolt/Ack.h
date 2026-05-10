#ifndef  THORSANVIL_NISSE_BOLT_ACK_H
#define  THORSANVIL_NISSE_BOLT_ACK_H

#include "NisseBoltConfig.h"

#include "ThorsSlack/SlackClient.h"
#include "NisseHTTP/Response.h"

namespace ThorsAnvil::Nisse::Bolt
{

class Ack
{
    ThorsAnvil::Nisse::HTTP::Response&      response;
    public:
        Ack(ThorsAnvil::Nisse::HTTP::Response& response);
        void operator()() const;
        void operator()(int responseCode) const;
};

}

#endif
