#ifndef  THORSANVIL_NISSE_BOLT_SAY_H
#define  THORSANVIL_NISSE_BOLT_SAY_H

#include "NisseBoltConfig.h"

#include "ThorsSlack/APIChatMessage.h"
#include "ThorsSlack/SlackClient.h"

#include <string>
#include <optional>

namespace ThorsAnvil::Nisse::Bolt
{

using OptString = std::optional<std::string>;

struct Where
{
    OptString               channel;
    std::string             ts;
};

class Say
{
    Slack::SlackClient&     client;
    Where                   where;
    public:
        Say(Slack::SlackClient& client, Where where);
        void operator()(std::string const& message) const;
        void operator()(Where const& where, std::string const& message) const;
        void operator()(Slack::BlockKit::Blocks const&) const;
        void operator()(Where const& where, Slack::BlockKit::Blocks const&) const;
    private:
        void sendMessage(Slack::API::Chat::POSTMessage message) const;
};

}

#endif
