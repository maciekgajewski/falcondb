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

/// Most functions in the interface are asynnchronous, with result return via callback.
/// Caller should not make any assumption about when the callback is going to be called.
/// It may be called immediately before the call resutns, or at later time in another thread.
/// The callback should move the result back to frontend thread and returns ASAP.


namespace falcondb { namespace interfaces {


/// Communication channel to a system object
class channel
{
public:

    typedef
    std::function<void (const error_message& error, const optional_document& result)>
    command_result_callback;

    /// Send command to associated object.
    virtual void post(
        const std::string& command,
        document&& param,
        command_result_callback&& callback) = 0;

    /// Closes the comm channel
    virtual ~channel() {}
};

typedef std::shared_ptr<channel> channel_ptr;



/// Session - represents clients session with the engine.
/// All open channels ans sent messages belong to the session and are destroyed when the session is over.
class session
{
public:

    typedef
    std::function<void (const error_message& error, const channel_ptr& result)>
    open_channel_callback;

    /// Opens comm channel to object defined by path.
    /// Implementation of this call may be idempotent - ie subsequent openning of the same
    /// object will result the same handler.
    virtual void open(
        const std::string& path,
        const open_channel_callback& callback) = 0;

    virtual ~session() {}
};

typedef std::shared_ptr<session> session_ptr;

/// frontend's view on the dataabse engine
class engine
{
public:

    typedef
    std::function<void (const error_message& error, const session_ptr& result)>
    create_session_callback;

    /// Creates client's database session. The name must be unique in the socpe of the engine.
    virtual void create_session(
        const std::string& session_name,
        const create_session_callback& callback) = 0;
};

// obsolete ////////////////////////////

typedef std::function<void (const error_message& error, const document_list& result)> result_handler;

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
        const document& params,
        const result_handler& result) = 0;

    // DEBUG: dumps content to stdout
    virtual void dump() = 0;
};

typedef std::shared_ptr<database> database_ptr;



}}

#endif
