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

#include "engine/engine.hpp"

#include <boost/filesystem.hpp>

#include <algorithm>

namespace falcondb { namespace engine {

namespace detail {

class db_handler : public interfaces::database
{
public:

    db_handler(boost::asio::io_service& io_service, interfaces::database_backend_ptr db)
    : _io_service(io_service), _db(db)
    { }

    virtual bool post(
        const std::string& command,
        const bson_object& params,
        const result_handler& result)

    {
        // TODO
    }

private:

   boost::asio::io_service& _io_service ;
   interfaces::database_backend_ptr _db;
};

} // detail

engine_impl::engine_impl(const engine_config& config, interfaces::storage_backend& backend)
:
    _config(config),
    _storage_backend(backend)
{
}

engine_impl::~engine_impl()
{
    _work.reset();
    if (_thread) _thread->join();
}

void engine_impl::run()
{
    namespace bfs = boost::filesystem3;

    std::cout << "initializing databases from " << _config.data_dir << std::endl;
    bfs::directory_iterator it(_config.data_dir);
    for( ;it != bfs::directory_iterator(); ++it)
    {
        if (it->status().type() == bfs::directory_file)
        {
            std::cout << "trying " << it->path() << " ... ";
            try
            {
                interfaces::database_backend_ptr db = _storage_backend.open_database(it->path().string());
                // TODO ...
                std::cout << "OK";

            }
            catch(const std::exception& e)
            {
                std::cout << "Failed: " << e.what();
            }
            std::cout << std::endl;
        }
    }
    std::cout << _databases.size() << " databases loaded" << std::endl;

    // start worker thread
    _work.reset(new boost::asio::io_service::work(_io_service));
    _thread.reset(new boost::thread([this] { _io_service.run(); }));
}

std::vector<std::string> engine_impl::get_databases()
{
    std::vector<std::string> result;
    std::transform(_databases.begin(), _databases.end(),
        std::back_inserter(result), [](const database_map::value_type& p) { return p.first; });

    return result;
}

interfaces::database_ptr engine_impl::get_database(const std::string& db_name)
{
    return interfaces::database_ptr();
}

} }
