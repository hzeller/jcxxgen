
To run example make sure to have [nlohmann/json] installed on your system.

This generates structs from the [textdocs.yaml](./textdocs.yaml)

```yaml
TextDocument:
  url: string
  bytes: integer

ReadingList:
  to_read+: TextDocument
  other: object

OptionallyVersionedTextDocument:
  <: TextDocument
  version?: integer
```

And uses them in the [testing-textdocs.cc](./testing-textdocs.cc) example
program.

[nlohmann/json]: https://github.com/nlohmann/json
