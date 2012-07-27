add_executable(new_doc
    main.cpp

    dynamic_document.hpp
    json_writer.cpp json_writer.hpp
    json_parser.cpp json_parser.hpp
)

target_link_libraries(new_doc
    utils
    bson

    ${BOOST_LIBS}
    boost_date_time
    boost_thread
)


