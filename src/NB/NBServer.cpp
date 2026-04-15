#include "NBServer.h"

#include "NisseServer/NisseServer.h"
#include <filesystem>

using namespace ThorsAnvil::NisseBolt;


TASock::SServerInfo NBServer::getServerInit(std::filesystem::path const& certPath, int port)
{
    // Assume the "full certificate chain" and "private key" files are on the path provided.
    TASock::CertificateInfo     certificate{std::filesystem::canonical(certPath) /= "fullchain.pem",
                                            std::filesystem::canonical(certPath) /= "privkey.pem"
                                           };
    TASock::SSLctx              ctx{TASock::SSLMethodType::Server, certificate};
    // Return an SSL configured port
    return TASock::SServerInfo{port, std::move(ctx)};
}

NBServer::NBServer(NBConfig const& config)
    : NisseServer(workerCount)
    , control(*this)
{
    listen(getServerInit(std::filesystem::path{config.certRoot} /= config.domainName, config.port), control);
}
