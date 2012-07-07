add_executable(bson_test
    main.cpp
    bson.cpp
    another.cpp
)

target_link_libraries(bson_test
    bson

    boost_unit_test_framework
    boost_thread
)
