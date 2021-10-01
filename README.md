JSON / C++ object schema
========================

Convert a schema, written in YAML to a C++ struct hierarchy boilerplate that
can be serialized and deserialized. Generates structs and the necessary
serialization functions to work seamlessly with [nlohmann/json].

In particular class hierarchies, whose objects are serialized to a flat
json object is something needed in some contexts, so having the tedious
mapping to C++ done with a code generator is nice.

### YAML Schema

A struct is defined with a name and properties. The properties have types, of
which the **basic types** are `integer`, `boolean` and `string`.

```yaml
TextDocument:
  url: string
  bytes: integer
```

You can **use defined types** then in other structs; here we use the
`TextDocument` referred to above. If we don't know the type or just
want to store plain json, the type `object` can be used. All of these are
stored by value.

**Repeated** (vector) fields can be marked with a `+` at the end of the
property name.

```yaml
ReadingList:
  to_read+: TextDocument
  other: object
```

Structs can be **inherited**; you can do this by using `<: SuperClass` in
the property field. **Optional** fields are indicated with a `?` at the
end of the property name.


```yaml
OptionallyVersionedTextDocument:
  <: TextDocument
  version?: integer
```

You can combine `+` and `?` for **optional repeated** fields. Just a repeated
field is _always_ serialized while an optional field will not show up in the
If need be, even **multiple inheritance** works by adding additional `<:`.

### Output

The output are structs with each of the properties stored by value; repeated
fields are `std::vector<theType>`. Also the necessary boilerplate is generated
to seamlessly convert the types to and from json with [nlohmann/json].

Here is a tiny example, for more check out the [example/](./example) dirctory.

```C++
  nlohmann::json in_json = { { "url", "https://github.com/hzeller/jcxxgen"},
                             { "bytes", 12345 } };
  TextDocument td = in_json;  // Automatic conversion from json to object
```

### Usage

Input is the yaml schema file, output is a c++ header file.

```
usage: jcxxgen [options] <protocol-spec-yaml>
Options:
  -o <filename>     : Output to filename
  -j <json-include> : Include path to json.hpp, including brackets <> or quotes "" around.
                      Default: '<nlohmann/json.hpp>'
```

### Work in Progress
Note, this is work in progress, things might change.

[nlohmann/json]: https://github.com/nlohmann/json
