#include "NBServer.h"

#include <iostream>

int main(int /*argc*/, char* /*argv*/[])
{

    try
    {
        std::cout << "NB\n";

        using ThorsAnvil::NisseBolt::NBServer;
        using ThorsAnvil::NisseBolt::NBConfig;
        NBServer    server(NBConfig{});
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
