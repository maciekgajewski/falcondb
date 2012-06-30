add_library(console_frontend

    console_frontend.cpp console_frontend.hpp
)

target_link_libraries(console_frontend

    boost_system
    pthread
    readline
)
