add_library(index_btree
    btree.cpp btree.hpp
    index.cpp index.hpp
    index_type.cpp index_type.hpp
)

target_link_libraries(index_btree
    utils
    document

)
