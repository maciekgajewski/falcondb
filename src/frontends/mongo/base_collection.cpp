/*
FalconDB, a database
Copyright (C) 2012 Kamil Zbrog <kamil.zbrog at gmail dot com>

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

#include "base_collection.hpp"

#include "utils/log.hpp"

#include "bson/bsonobjbuilder.ipp"

#include <boost/assign/list_of.hpp>
#include <boost/system/error_code.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>

namespace falcondb { namespace frontend { namespace mongo {

void base_collection::post(
    const std::string& command,
    const bson_object_list& params,
    const result_handler& handler)
{
    if (command == "query") {
        handle_query(params, handler);
    } else if (command == "insert") {
        handle_insert(params, handler);
    } else {
        handler(error_message(), bson_object_list());
    }
}

}}}
