/** @file bson.h

    Main bson include file for mongodb c++ clients. MongoDB includes ../db/jsobj.h instead. 
    This file, however, pulls in much less code / dependencies.

    @see bsondemo
*/

/*
 *    Copyright 2009 10gen Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
   Main include file for C++ BSON module when using standalone (sans MongoDB client).

   "BSON" stands for "binary JSON" -- ie a binary way to represent objects that would be
   represented in JSON (plus a few extensions useful for databases & other languages).

   http://www.bsonspec.org/
*/

#pragma once

#include "bson/bsonelement.h"
#include "bson/bsonobj.h"
#include "bson/bsonobjbuilder.h"
#include "bson/bsonobjiterator.h"
#include "bson/bsontypes.h"
#include "bson/bson-inl.h"
#include "bson/oid.h"
#include "bson/util/builder.h"

// namespacte translation (temporary, until code cleaned up)
namespace falcondb {
    typedef mongo::BSONObj bson_object;
    typedef std::vector<bson_object> bson_object_list;
}