add_library(engine
    engine.cpp engine.hpp
    database_impl.cpp database_impl.hpp
    command_processor.cpp command_processor.hpp
)

target_link_libraries(engine
    boost_filesystem
    util
)
