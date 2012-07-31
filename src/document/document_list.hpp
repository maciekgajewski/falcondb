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

#ifndef FALCONDB_DOCUMENT_LIST_HPP
#define FALCONDB_DOCUMENT_LIST_HPP

#include "document/detail/variants.hpp"

namespace falcondb {

class document_list : public detail::raw_document_list
{
public:
    document_list() = default; // wooohoo!
    document_list(const detail::raw_document_list& raw);
    document_list(detail::raw_document_list&& raw);
    document_list(const document& d);
    document_list(document&& d);

    // collapse to std::vector of specific type
    // requirement: there must be a compatible document::as<T>()
    template<typename T>
    std::vector<T> to_list_of() const;

    // builds document_lisdt from any other vector
    // requirement: there must be a compatible document::from<T>()
    template<typename T>
    static
    document_list from(const std::vector<T>& input);


    // io
    std::string to_json() const;
    static document_list from_json(const std::string& s);
};

}

#endif
