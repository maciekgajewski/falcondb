set(LevelDB_SEARCH_PATHS /usr/lib/ /usr/lib64)
set(LevelDB_INC_SEARCH_PATHS /usr/include)

find_path(LEVELDB_INCLUDE_DIR 
    NAMES leveldb/db.h 
    HINTS ${LevelDB_INC_SEARCH_PATHS})

find_library(LEVELDB_LIBRARY 
    NAMES leveldb 
    HINTS ${LevelDB_SEARCH_PATHS})

find_library(SNAPPY_LIBRARY
    NAMES snappy
    HINTS ${LevelDB_SEARCH_PATHS})

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LevelDB
    REQUIRED_VARS LEVELDB_LIBRARY LEVELDB_INCLUDE_DIR)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Snappy
    REQUIRED_VARS SNAPPY_LIBRARY)

mark_as_advanced(LEVELDB_LIBRARY SNAPPY_LIBRARY)
