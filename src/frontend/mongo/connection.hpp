#ifndef FALCONDB_FRONTEND_MONGO_CONNECTION_HPP
#define FALCONDB_FRONTEND_MONGO_CONNECTION_HPP

#include "message.hpp"

#include "interfaces/engine.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace falcondb { namespace frontend { namespace mongo {

class connection : public boost::enable_shared_from_this<connection>
{
public:
    typedef boost::shared_ptr<connection> pointer;

    connection(boost::asio::io_service& io_service, interfaces::engine& engine);

    boost::asio::ip::tcp::socket& socket() { return _socket; }

    void start();

private:
    void handle_read_header(const boost::system::error_code& e, const message::pointer& msg);
    void handle_read_body(const boost::system::error_code& e, const message::pointer& msg);
    void handle_write_msg(const boost::system::error_code& e);

    void handle_query_msg(const message::pointer& msg);
    void handle_insert_msg(const message::pointer& msg);

    bson_object_list handle_admin_command(query_message& query_msg);
    bson_object_list handle_db_command(const std::string& db_name, query_message& query_msg);
    void handle_db_query(const message::pointer& msg);

    void post_command(const std::string& db_name, const std::string& command, bson_object param);
    void result_handler(const std::string& operation, const interfaces::error_message& err, const bson_object_list& data);
    void send_reply(const message::pointer& msg, const bson_object_list& obj_list);
    void handle_db_query(const std::string& dbname, const message::pointer& msg, const query_message& query_msg);

    boost::asio::ip::tcp::socket _socket;

    interfaces::engine& _engine;
};

}}}

#endif