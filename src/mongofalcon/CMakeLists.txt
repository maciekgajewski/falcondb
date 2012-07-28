add_executable(mongofalcon
    main.cpp
)

target_link_libraries(mongofalcon
    bson
    frontend_mongo 
    backend_leveldb
    engine
)


