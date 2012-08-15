add_executable(ifalcon
    main.cpp
)

target_link_libraries(ifalcon

    console_frontend
    backend_leveldb
    engine
    document
)


