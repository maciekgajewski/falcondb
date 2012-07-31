falcondb
========

Document store database similar to MongoDB.

why?
====

We use MongoDB in various projects. We love it. We hate it.
We would like to create something slightly more loveable and less hateable.

planned features
================

* Indexable, schemaless document store.
* Excellent for data mining.
* Modular architecture with pluggable storage backends, frontends, command implementations, index types etc.
* Mongo-compatible frontend allows it to be used as a drop-in replacement for MongoDB in simple applications.
* Build-in JavaScript interpreter for server-side code execution.
* Multi-threaded core allowing for concurrent R/W access to the data by multiple clients.
