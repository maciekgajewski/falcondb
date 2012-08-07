add_library(engine
    engine.cpp engine.hpp
    database.cpp database.hpp
    command_processor.cpp command_processor.hpp
    commands.cpp commands.hpp
    document_storage.cpp document_storage.hpp
)

target_link_libraries(engine
    utils
    ${Boost_LIBRARIES}
    index_btree
)
