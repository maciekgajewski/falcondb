Document abstraction.

It turned out that the quality of mongo BSON inplementation leaves a lot to be desired.
And there is no other implementation.

Therefore, suddendly, there is no obvious choice for document representation.
This library will create a document abstraction wrapped around some generic JSON lib.
In the future we may change the underlying implementattion.
