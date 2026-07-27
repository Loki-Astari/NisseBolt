#ifndef  THORSANVIL_NISSE_BOLT_ACK_H
#define  THORSANVIL_NISSE_BOLT_ACK_H

#include "NisseBoltConfig.h"

#include "ThorsSlack/Client.h"
#include "NisseHTTP/Response.h"

namespace ThorsAnvil::Nisse::Bolt
{

class Ack
{
        static ThorsAnvil::Serialize::PrinterConfig     const printerConfig;

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
            response.addHeader("Content-Type", "application/json");
            std::size_t size = ThorsAnvil::Serialize::jsonStreamSize(message);
            response.body(size)
                << ThorsAnvil::Serialize::jsonExporter(message, printerConfig);
        }
};

}

#endif
