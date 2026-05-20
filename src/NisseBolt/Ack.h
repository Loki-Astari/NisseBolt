#ifndef  THORSANVIL_NISSE_BOLT_ACK_H
#define  THORSANVIL_NISSE_BOLT_ACK_H

#include "NisseBoltConfig.h"

#include "ThorsSlack/SlackClient.h"
#include "NisseHTTP/Response.h"

namespace ThorsAnvil::Nisse::Bolt
{

class Ack
{
        class JsonHeader: public ThorsAnvil::Nisse::HTTP::HeaderResponse
        {
            public:
                JsonHeader()
                {
                    add("Content-Type", "application/json");
                }
        };
        static ThorsAnvil::Serialize::PrinterConfig     const printerConfig;
        static JsonHeader                               const jsonHeaders;

        ThorsAnvil::Nisse::HTTP::Response&      response;
    public:
        Ack(ThorsAnvil::Nisse::HTTP::Response& response);
        void operator()() const;
        void operator()(int responseCode) const;

        void operator()(int responseCode, std::string const& message) const;
        void operator()(int responseCode, ThorsAnvil::Slack::BlockKit::Blocks const& message) const;
        void operator()(std::string const& message) const;
        void operator()(ThorsAnvil::Slack::BlockKit::Blocks const& message) const;

        template<typename T>
        void operator()(T const& message) const
        {
            response.addHeaders(jsonHeaders);
            std::size_t size = ThorsAnvil::Serialize::jsonStreanSize(message);
            response.body(size)
                << ThorsAnvil::Serialize::jsonExporter(message, printerConfig);
        }
};

}

#endif
