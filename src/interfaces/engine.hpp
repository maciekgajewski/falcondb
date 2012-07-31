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

#ifndef FALCONDB_INTERFACES_ENGINE_HPP
#define FALCONDB_INTERFACES_ENGINE_HPP

#include "document/document.hpp"

#include <utils/error_message.hpp>

#include <vector>
#include <string>
#include <memory>
#include <functional>

// These interfaces are the API exposed to db frontend

namespace falcondb { namespace interfaces {

class database;
class command_context;

typedef std::shared_ptr<database> database_ptr;

typedef std::function<void (const error_message& error, const document& result)> result_handler;
typedef std::function<void (const document&, const result_handler&, command_context&)> command_handler;

/// API exposed toward frontends.
class engine
{
public:
    virtual std::vector<std::string> get_databases() = 0;
    virtual database_ptr get_database(const std::string& db_name) = 0;
    virtual database_ptr create_database(const std::string& db_name) = 0;
    virtual void drop_database(const std::string& db_name) = 0;
};

class database
{
public:

    /// Puts command into datyabases' command queue.
    /// \returns false if the queue is full
    /// \param command the command
    /// \param params command params
    /// \param handler completion handler, called in one of engine threads
    virtual bool post(
        const std::string& command,
        const document_list& params,
        const result_handler& result) = 0;

    // DEBUG: dumps content to stdout
    virtual void dump() = 0;
};

typedef std::shared_ptr<database> database_ptr;

}}

#endif
