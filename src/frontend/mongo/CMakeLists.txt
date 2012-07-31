add_library(frontend_mongo
    collection.cpp collection.hpp
    engine.cpp engine.hpp
    frontend.cpp frontend.hpp
    connection.cpp connection.hpp
    message.hpp
)

target_link_libraries(frontend_mongo
    utils
    bson

    ${Boost_LIBRARIES}
    jsoncpp
    pthread
)
