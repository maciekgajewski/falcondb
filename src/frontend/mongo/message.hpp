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

#ifndef FALCONDB_FRONTENT_MONGO_MESSAGE_HPP
#define FALCONDB_FRONTENT_MONGO_MESSAGE_HPP

#include "utils/range.hpp"

#include "bson/bsonobj.h"

#include <sstream>
#include <memory>

namespace falcondb { namespace frontend { namespace mongo {

struct coder
{
    coder(std::ostream& out)
    : _out(out)
    {
    }

    template<typename T>
    void put(const T& data)
    {
        _out.write(reinterpret_cast<const char*>(&data), sizeof(data));
    }

    std::ostream& _out;
};

struct message
{

struct op_code {

enum type {
    REPLY = 1,
    QUERY = 2004,
    INSERT = 2002
};

};

#pragma pack(push, 1)
struct header
{
    std::uint32_t _msglen;
    std::uint32_t _reqId;
    std::uint32_t _resId;
    std::uint32_t _opCode;
    std::uint32_t _reserved;

    const char* data() const { return reinterpret_cast<const char*>(&_msglen); }
};

#pragma pack(pop)
    enum { header_length = sizeof(header) };

    message()
    {
        _header._msglen = header_length;
    }

    header _header;
    std::string _body;


    typedef std::shared_ptr<message> pointer;
};

inline std::ostream& operator <<(std::ostream& out, message::op_code::type op_code)
{
    switch(op_code) {
        case message::op_code::REPLY: return out << "REPLY";
        case message::op_code::QUERY: return out << "QUERY";
        case message::op_code::INSERT: return out << "INSERT";
        default: return out << static_cast<std::uint32_t>(op_code);
    }
}

inline std::ostream& operator <<(std::ostream& out, const message::header& header)
{
    out
        << "msg header"
        << " msgLen: " << header._msglen
        << " regId:  " << header._reqId
        << " respId: " << header._resId
        << " opCode: " << static_cast<message::op_code::type>(header._opCode)
        << " reserv: " << header._reserved;

    return out;
}

struct db_message
{
    db_message(const message& msg)
    : _msg(msg)
    , _data(msg._body)
    , _next_obj_data(_data.begin() + strlen(_data.begin()) + 1)
    {
    }

    const char * get_ns() const { return _data.begin(); }

    bool has_more() const { return _next_obj_data != nullptr; }

    ::mongo::BSONObj next_obj()
    {
        ::mongo::BSONObj obj(_next_obj_data);

        _next_obj_data += obj.objsize();

        if (_next_obj_data >= _data.end()) {
            _next_obj_data = nullptr;
        }

        return obj;
    }

    const message &_msg;
    const range _data;
    const char *_next_obj_data;
};

struct query_message : public db_message
{
    query_message(const message& msg)
    : db_message(msg)
    , _number_to_skip(*reinterpret_cast<const std::uint32_t*>(_next_obj_data))
    , _number_to_return(*reinterpret_cast<const std::uint32_t*>(_next_obj_data+4))
    {
        _next_obj_data += 2 * (sizeof(std::uint32_t));
    }

    std::uint32_t number_to_skip() const { return _number_to_skip; }
    std::uint32_t number_to_return() const { return _number_to_return; }

    const std::uint32_t _number_to_skip;
    const std::uint32_t _number_to_return;
};

} } }


#endif
