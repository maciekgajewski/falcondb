add_library(engine
    engine.cpp engine.hpp
    database_impl.cpp database_impl.hpp
    command_processor.cpp command_processor.hpp
    commands.cpp commands.hpp
)

target_link_libraries(engine
    utils

    boost_filesystem
)
