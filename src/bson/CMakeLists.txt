add_library(bson
    bson.hpp
    bsonobj.hpp bsonobj.ipp
    bsonelement.hpp bsonelement.ipp
    bsonobjbuilder.hpp bsonobjbuilder.ipp
    bsonobjiterator.hpp
    json.hpp json.cpp
    jsobj.cpp
    oid.cpp oid.hpp
    base64.cpp base64.hpp
    nonce.cpp nonce.hpp
    lex_num_cmp.hpp
    allocators.hpp
    buf_builder.hpp
    string_builder.hpp
    atomic.hpp
    ordering.hpp
    string_data.hpp
    embedded_builder.hpp
    optime.hpp
    misc.hpp
    date.hpp
    enums.hpp
    size_tracker.hpp
)
target_link_libraries(bson ${Boost_LIBRARIES})
