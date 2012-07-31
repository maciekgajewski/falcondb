Document abstraction.

It turned out that the quality of mongo BSON inplementation leaves a lot to be desired.
And there is no other implementation.

Therefore, suddendly, there is no obvious choice for document representation.

This library creates JSON document abstraction.
Unlike all the other inmplementation, this one is based around
generic C++ types, representing array as std::vector, object as std::map,
scalar value as variant and general document objct as variant of all the above.
