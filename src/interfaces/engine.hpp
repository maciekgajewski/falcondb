#ifndef FALCONDB_INTERFACES_ENGINE_HPP
#define FALCONDB_INTERFACES_ENGINE_HPP

#include "bson/bson.hpp"

#include <boost/optional.hpp>

#include <vector>
#include <string>
#include <memory>
#include <functional>

namespace falcondb { namespace interfaces {

class database;
class collection;
class database_backend;

typedef std::shared_ptr<database_backend> database_backend_ptr;

typedef boost::optional<std::string> error_message;
typedef std::function<void (const error_message& error, const bson_object_list& result)> result_handler;
typedef std::function<void (const bson_object&, const result_handler&, const database_backend_ptr&)> command_handler;

class engine
{
public:
    virtual std::vector<std::string> get_databases() = 0;
    virtual std::shared_ptr<database> get_database(const std::string& db_name) = 0;
    virtual void create_database() = 0;
    virtual void drop_database() = 0;
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
        const bson_object& params,
        const result_handler& result) = 0;
};

typedef std::shared_ptr<database> database_ptr;

}}

#endif
