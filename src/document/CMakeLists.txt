add_library(document
    document.cpp document.hpp)

target_link_libraries(document
    utils

    jsoncpp # ubuntu package: libjsoncpp
)
