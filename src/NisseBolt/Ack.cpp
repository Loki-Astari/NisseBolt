#include "Ack.h"

using namespace ThorsAnvil::Nisse::Bolt;

Ack::Ack(Slack::SlackClient& client, ThorsAnvil::Nisse::HTTP::Response& response)
    : client(client)
    , response(response)
{}

void Ack::operator()() const
{
    response.body(0);
}

void Ack::operator()(int responseCode) const
{
    response.setStatus(responseCode);
    response.body(0);
}
