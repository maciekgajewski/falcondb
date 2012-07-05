add_library(console_frontend

    frontend.cpp frontend.hpp
    command_dispatcher.cpp command_dispatcher.hpp
)

target_link_libraries(console_frontend
    utils

    boost_system
    boost_thread
    pthread
    readline
)
