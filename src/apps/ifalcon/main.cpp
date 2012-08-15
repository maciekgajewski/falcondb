/*
FalconDB, a database
Copyright (C) 2012 Maciej Gajewski <maciej.gajewski0 at gmail dot com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "frontends/console/frontend.hpp"

#include "backends/leveldb/backend.hpp"

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

    backend_leveldb::backend backend;

    dbengine::engine_config config = { arg };
    dbengine::engine engine(config, backend);

    console_frontend::frontend frontend(engine);

    engine.run();

    frontend.execute();
}
