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

#ifndef FALCONDB_DETAIL_DOCUMENT_VARIANTS_HPP
#define FALCONDB_DETAIL_DOCUMENT_VARIANTS_HPP

#include "document/document_scalar.hpp"


namespace falcondb {

class document;
class document_object;
class document_list;

namespace detail {

typedef boost::variant<
    //boost::recursive_wrapper< raw_document_scalar >,
    document_scalar,
    //raw_document_scalar,
    boost::recursive_wrapper< document_list >,
    boost::recursive_wrapper< document_object >
    > raw_document_any;

typedef std::map<std::string, document> raw_document_map;
typedef std::vector<document> raw_document_list;

}}

#endif
