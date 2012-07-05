#include "console_frontend/frontend.hpp"

#include "backend_nessdb/backend.hpp"

#include "dbengine/engine.hpp"

using namespace falcondb;

void help()
{
    std::cout << "usage: ifalcon DBPATH" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        help();
        return 1;
    }
    std::string arg = argv[1];
    if (arg == "--help" || arg == "-help")
    {
        help();
        return 1;
    }

    backend_nessdb::backend backend;

    dbengine::engine_config config = { arg };
    dbengine::engine engine(config, backend);

    console_frontend::frontend frontend(engine);

    engine.run();

    frontend.execute();
}
