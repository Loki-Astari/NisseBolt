#ifndef THORSANVIL_SLACK_SLACKSTREAM_H
#define THORSANVIL_SLACK_SLACKSTREAM_H

#include "ThorsSlackConfig.h"
#include "NisseHTTP/ClientHTTP.h"
#include "ThorsSocket/SocketUtil.h"

namespace ThorsAnvil::Slack
{

class Stream
{
    ThorsAnvil::ThorsSocket::SSLctx            ctx;
    ThorsAnvil::Nisse::HTTP::ClientHTTP        client;
    public:
        Stream()
            : ctx{ThorsAnvil::ThorsSocket::SSLMethodType::Client}
            , client(ThorsAnvil::ThorsSocket::SSocketInfo{"slack.com", 443, ctx, ThorsAnvil::ThorsSocket::DeferAccept::No}, ThorsAnvil::Nisse::HTTP::Version::HTTP1_1)
        {}

        ThorsAnvil::Nisse::HTTP::ClientHTTP& getClient() {return client;}
};

}

#endif
