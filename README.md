JSON / C++ object schema
========================

_Work in progress, things are in flux._

Convert a schema, written in YAML to a C++ struct hierarchy boilerplate that
can be serialized and deserialized. Generates structs and the necessary
serialization functions to work seamlessly with [nlohmann/json].

In particular class hierarchies, whose objects are serialized to a flat
json object is something needed in some contexts, so having the tedious
mapping to C++ done with a code generator is nice.

### YAML Schema

Description TDB. See in [lsp-protocol.yaml](example/lsp-protocol.yaml) for now
to see inheritance, default values and object properties.

### Usage

```
usage: jcxxgen [options] <protocol-spec-yaml>
Options:
  -o <filename>     : Output to filename
  -j <json-include> : Include path to json.hpp, including brackets <> or quotes "" around.
                      Default: '<nlohmann/json.hpp>'
```

### Work in Progress
Note, this is work in progress, things might change. In particular optional
fields are not implemented yet.

[nlohmann/json]: https://github.com/nlohmann/json
