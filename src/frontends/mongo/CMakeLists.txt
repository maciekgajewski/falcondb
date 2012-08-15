add_library(frontend_mongo
    base_collection.cpp base_collection.hpp
    admin_collection.cpp admin_collection.hpp
    collection_group.cpp collection_group.hpp
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
