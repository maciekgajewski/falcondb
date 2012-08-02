add_executable(btree_test
    main.cpp
    tree.cpp
)

target_link_libraries(btree_test
    index_btree

    boost_unit_test_framework
    boost_thread
)
