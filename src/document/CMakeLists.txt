add_library(document
    document.cpp document.hpp
    json_parser.cpp json_parser.hpp
    json_writer.cpp json_writer.hpp
)

target_link_libraries(document
    utils
)
