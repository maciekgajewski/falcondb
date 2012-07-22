add_executable(bson_test
    main.cpp
    bson.cpp
    bson2.cpp
)

target_link_libraries(bson_test
    bson

    boost_unit_test_framework
    boost_thread
)
