#ifndef THORSANVIL_NISSEBOLT_NB_H
#define THORSANVIL_NISSEBOLT_NB_H

#include "NBConfig.h"

#include "NisseServer/NisseServer.h"
#include "NisseHTTP/PyntHTTPControl.h"

#include <filesystem>


namespace ThorsAnvil::NisseBolt
{

using ThorsAnvil::Nisse::Server::NisseServer;
using ThorsAnvil::Nisse::HTTP::PyntHTTPControl;


class NBServer: public NisseServer
{
    static constexpr std::size_t workerCount = 4;

    PyntHTTPControl             control;

    TASock::SServerInfo getServerInit(std::filesystem::path const& certPath, int port);

    public:
        NBServer(NBConfig const& config);
};

}

#endif
