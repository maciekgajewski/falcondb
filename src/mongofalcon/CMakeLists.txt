add_executable(mongofalcon
    main.cpp
)

target_link_libraries(mongofalcon
    frontend_mongo 
    backend_leveldb
    engine
)


