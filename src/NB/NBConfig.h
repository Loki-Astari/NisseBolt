#ifndef THORSANVIL_NISSEBOLT_NBCONFIG_H
#define THORSANVIL_NISSEBOLT_NBCONFIG_H

#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/SerUtil.h"

#include <string>

namespace ThorsAnvil::NisseBolt
{

struct NBConfig
{
    int                         port;           // Port to listen on
    std::string                 domainName;     // Name of the domain
    std::string                 certRoot;       // SSL certificate root (converted to filesystem::path at use site)

    std::string                 botToken;       // Slack bot token (xoxb-...)
    std::string                 signingSecret;  // Slack signing secret for HMAC verification
    std::string                 basePath = "/slack"; // Mount point for the four Slack routes
};

}

ThorsAnvil_MakeTrait(ThorsAnvil::NisseBolt::NBConfig, port, domainName, certRoot, botToken, signingSecret, basePath);


#endif
