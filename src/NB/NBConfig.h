#ifndef THORSANVIL_NISSEBOLT_NBCONFIG_H
#define THORSANVIL_NISSEBOLT_NBCONFIG_H

#include <string>
#include <filesystem>

namespace ThorsAnvil::NisseBolt
{

struct NBConfig
{
    int                         port;           // Port to listen on
    std::string                 domainName;     // Name of the domain
    std::filesystem::path       certRoot;       // The location of the SSL Certification (if any)
};

}

#endif
