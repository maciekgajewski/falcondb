add_executable(new_doc
    main.cpp

    dynamic_document.hpp
    document_serializer.hpp
    json_document_writer.cpp json_document_writer.hpp
)

target_link_libraries(new_doc
    utils

    ${BOOST_LIBS}
    boost_date_time
)


