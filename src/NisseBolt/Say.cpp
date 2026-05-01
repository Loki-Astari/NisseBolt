#include "Say.h"
#include "ThorSerialize/JsonThor.h"
#include "ThorsSlack/APIChatMessage.h"

using namespace ThorsAnvil::Nisse::Bolt;

Say::Say(Slack::SlackClient& client, Where where)
    : client{client}
    , where{std::move(where)}
{}

void Say::operator()(std::string const& message) const
{
    (*this)(where, message);
}

void Say::operator()(Where const& where, std::string const& message) const
{
    sendMessage(Slack::API::Chat::POSTMessage{
                                               .channel = where.channel.value_or("bad"),
                                               .text = message,
                                               //.blocks
                                               //.icon_emoji
                                               //.username
                                               .thread_ts = where.ts
                                             });
}

void Say::operator()(Slack::BlockKit::Blocks const& blocks) const
{
    (*this)(where, blocks);
}

void Say::operator()(Where const& where, Slack::BlockKit::Blocks const& blocks) const
{
    sendMessage(Slack::API::Chat::POSTMessage{
                                               .channel = where.channel.value_or("bad"),
                                               //.text
                                               .blocks = blocks,
                                               //.icon_emoji
                                               //.username
                                               .thread_ts = where.ts
                                             });
}

void Say::sendMessage(Slack::API::Chat::POSTMessage message) const
{
    Slack::API::Chat::POSTMessage::Reply   reply;
    if (!client.sendMessage(std::move(message), reply)) {
        std::cerr << "Failed: " << ThorsAnvil::Serialize::jsonExporter(reply) << "\n";
    }
}
