add_executable(bson_test
    main.cpp
    bson.cpp
    bson2.cpp
)

target_link_libraries(bson_test
    bson
)
