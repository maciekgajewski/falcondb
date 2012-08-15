if(COMPILE_NESSDB_BACKEND)
add_subdirectory(nessdb)
endif()
if(COMPILE_LEVELDB_BACKEND)
add_subdirectory(leveldb)
endif()



