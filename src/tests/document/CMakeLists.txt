add_executable(document_test
    main.cpp
)

target_link_libraries(document_test
    utils
    document
    bson

    ${BOOST_LIBS}
    boost_date_time
    boost_thread
)


