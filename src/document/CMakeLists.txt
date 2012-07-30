add_library(document
    document.cpp document.hpp
    document_scalar.cpp document_scalar.hpp document_scalar.ipp
    document_object.cpp document_object.hpp document_object.ipp
    document_list.cpp document_list.hpp document_list.ipp
    json_parser.cpp json_parser.hpp
    json_writer.cpp json_writer.hpp
    null_type.hpp
)

target_link_libraries(document
    utils

    boost_date_time
)
