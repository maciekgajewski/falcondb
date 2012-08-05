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

#ifndef FALCONDB_INTERFACES_COMMAND_CONTEXT_HPP
#define FALCONDB_INTERFACES_COMMAND_CONTEXT_HPP

#include "interfaces/index.hpp"

namespace falcondb { namespace interfaces {

class document_storage;

typedef std::map<std::string, index::unique_ptr> index_map;

/// Database tools provided to each executed database command
class command_context
{
public:

    /// Main data storage. Can be used to access document data
    virtual document_storage& get_data_storage() = 0;
    virtual index_map& get_indexes() = 0;
};


}} // namespaces

#endif
