#include "console_frontend/frontend.hpp"

#include "backend_nessdb/backend.hpp"

#include "engine/engine.hpp"

using namespace falcondb;

int main(int argc, char** argv)
{
    console_frontend::frontend frontend;
    backend_nessdb::backend backend;

    engine::engine_config config = { "./data" };
    engine::engine_impl engine(config, backend);

    engine.run();

    frontend.execute();
}
