add_library(frontend_mongo
    frontend.cpp frontend.hpp
    connection.cpp connection.hpp
    message.hpp
)

target_link_libraries(frontend_mongo
    utils
    bson

    ${Boost_LIBRARIES}
    pthread
)
