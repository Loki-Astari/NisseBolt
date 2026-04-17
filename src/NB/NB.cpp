#include "NBServer.h"
#include "NBContext.h"
#include "NBConfig.h"

#include <iostream>
#include <fstream>

namespace TS = ThorsAnvil::Serialize;
namespace NB = ThorsAnvil::NisseBolt;

int main(int /*argc*/, char* /*argv*/[])
{
    std::ifstream   configFile("bot.config");
    NB::NBConfig    config = TS::jsonBuilder<NB::NBConfig>(configFile);

    try
    {
        std::cout << "NB\n";

        NB::NBServer    server(config);
        server.run();
    }
    catch (std::exception const& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        throw;
    }
    catch (...)
    {
        std::cerr << "Exception: Unknown\n";
        throw;
    }
}
