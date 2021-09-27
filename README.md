JSON / C++ object schema
========================

_Work in progress, things are in flux._

Convert a schema, written in YAML to a C++ struct hierarchy boilerplate that
can be serialized and deserialized. Generates structs and the necessary
serialization functions to work seamlessly with [nlohmann/json].

In particular class hierarchies, whose objects are serialized to a flat
json object is something needed in some contexts, so having the tedious
mapping to C++ done with a code generator is nice.

The current experimental yaml file represents part of the LSP schema.

To run example make sure to clone with submodules or have [nlohmann/json]
installed on your system.

### YAML Schema

Description TDB. See in [lsp-protocol.yaml](example/lsp-protocol.yaml) for now
to see inheritance, default values and object properties.

### Work in Progress
Note, this is work in progress, things might change. In particular optional
fields are not implemented yet.

[nlohmann/json]: https://github.com/nlohmann/json
