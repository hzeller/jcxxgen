#include <regex>
#include <fstream>
#include <unordered_map>
#include <iostream>
#include <string>
#include <memory>

// Interface. Currently private, but could be moved to a header if needed.
struct Location {
  const char *filename;
  int line;
};
inline std::ostream& operator <<(std::ostream& out, const Location& loc) {
  return out << loc.filename << ":" << loc.line << ": ";
}

struct ObjectType;

struct Property {
  Property(const Location &loc, const std::string &name, bool is_optional)
    : location(loc), name(name), is_optional(is_optional) {}

  bool EqualNameValue(const Property &other) const {
    return other.name == name;
  }

  Location location = { "<>", 0};  // Where it is defined
  std::string name;               // e.g. "connection_by_name"
  bool is_optional;

  std::string default_value;

  // TODO: have alternative types
  std::string type;
  ObjectType* object_type = nullptr;
};

struct ObjectType {
  ObjectType(const Location &loc, const std::string &name)
    : location(loc), name(name) {}

  Location location;
  std::string name;

  std::string extends;   // name of the superclass
  std::vector<Property> properties;

  const ObjectType *superclass = nullptr;  // fixed up after all models read.
};

// Parse models from file. Return vector of models if successful, nullptr
// otherwise.
using ObjectTypeVector = std::vector<ObjectType *>;
std::unique_ptr<ObjectTypeVector> LoadObjectTypes(const char *filename);

static bool ParseObjectTypesFromFile(const char *filename,
                                     ObjectTypeVector *parsed_out) {
  static const std::regex emptyline_re("^[ \t]*(#.*)?");
  static const std::regex toplevel_object_re("^([a-zA-Z0-9_]+):");

  // For now, let's just read up to the first type and leave out alternatives
  static const std::regex property_re(
    "^[ \t]+([a-zA-Z_<]+)(\\?)?:[ ]*([a-zA-Z0-9_]+)[ ]*(=[ \t]*(.+))?");

  Location current_location = { filename, 0 };
  ObjectType *current_model = nullptr;
  std::ifstream in(filename);
  if (!in.good()) {
    std::cerr << "Can't open " << filename << "\n";
    return false;
  }
  std::string line;
  std::smatch matches;
  while (!in.eof()) {
    std::getline(in, line);
    current_location.line++;

    if (std::regex_match(line, emptyline_re))
      continue;

    if (std::regex_search(line, matches, toplevel_object_re)) {
      current_model = new ObjectType(current_location, matches[1]);
      parsed_out->push_back(current_model);
      continue;
    }

    if (!current_model) {
      std::cerr << current_location << "No ObjectType definition\n";
      return false;
    }
    if (!std::regex_search(line, matches, property_re)) {
      std::cerr << current_location << "This doesn't look like a property";
      return false;
    }

    if (matches[1] == "<") {
      current_model->extends = matches[3];
      continue;
    }

    Property property(current_location, matches[1], matches[2] == "?");
    property.type = matches[3];  // TODO: allow multiple
    property.default_value = matches[5];
    current_model->properties.push_back(property);
  }
  return true;
}

static bool ValidateTypes(ObjectTypeVector *object_types) {
  std::unordered_map<std::string, ObjectType *> typeByName;

  for (auto &o : *object_types) {
    // We only insert types as they come, so that we can make sure they are
    // used after being defined.
    auto inserted = typeByName.insert({o->name, o});
    if (!inserted.second) {
      std::cerr << o->location << "Duplicate name; previous defined in "
                << inserted.first->second->location << "\n";
      return false;
    }

    // Resolve superclass
    if (!o->extends.empty()) {
      const auto& found = typeByName.find(o->extends);
      if (found == typeByName.end()) {
        std::cerr << o->location << "Unknown superclass " << o->extends;
        return false;
      }
      o->superclass = found->second;
    }

    for (auto &p : o->properties) {
      const std::string &t = p.type;
      if (t == "object" || t == "array" || t == "string" ||
          t == "integer" || t == "boolean") {
        continue;
      }

      const auto& found = typeByName.find(t);
      if (found == typeByName.end()) {
        std::cerr << p.location << "Unknown object type '" << t << "'\n";
        return false;
      }
      p.object_type = found->second;
    }
  }
  return true;
}

std::unique_ptr<ObjectTypeVector> LoadObjectTypes(const char *filename) {
  std::unique_ptr<ObjectTypeVector> result(new ObjectTypeVector());

  if (!ParseObjectTypesFromFile(filename, result.get())) return nullptr;
  if (!ValidateTypes(result.get())) return nullptr;
  return result;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <protocol-spec-yaml>\n", argv[0]);
    return 1;
  }
  const char *filename = argv[1];
  auto objects = LoadObjectTypes(filename);
  if (!objects) {
    fprintf(stderr, "Couldn't parse spec\n");
    return 2;
  }

  FILE *out = stdout;
  fprintf(out, "// Don't modify. Generated from %s\n", filename);
  fprintf(out, "#pragma once\n"
          "#include <string>\n"
          "#include <nlohmann/json.hpp>\n\n");
  for (const auto& o : *objects) {
    if (o->extends.empty()) {
      fprintf(stdout, "struct %s {\n", o->name.c_str());
    } else {
      fprintf(stdout, "struct %s : public %s {\n",
              o->name.c_str(), o->extends.c_str());
    }
    for (const auto&p : o->properties) {
      std::string type;
      if (p.object_type) type = p.object_type->name;
      else if (p.type == "string") type = "std::string";
      else if (p.type == "integer") type = "int";
      else if (p.type == "object") type = "nlohmann::json";
      else if (p.type == "boolean") type = "bool";
      // TODO: optional and array
      if (type.empty()) {
        std::cerr << p.location << "Not supported yet '" << p.type << "'\n";
        continue;
      }
      fprintf(out, "  %s %s", type.c_str(), p.name.c_str());
      if (!p.default_value.empty())
        fprintf(out, " = %s", p.default_value.c_str());
      fprintf(out, ";\n");
    }

    // nlohmann::json serialization
    fprintf(out, "\n");
    fprintf(out, "  void Deserialize(const nlohmann::json &j) {\n");
    if (!o->extends.empty()) {
      fprintf(out, "    %s::Deserialize(j);\n",
              o->extends.c_str());
    }
    for (const auto&p : o->properties) {
      if (p.object_type == nullptr) {
        fprintf(out, "    j.at(\"%s\").get_to(%s);\n",
                p.name.c_str(), p.name.c_str());
      } else {
        fprintf(out, "    %s.Deserialize(j.at(\"%s\"));\n",
                p.name.c_str(), p.name.c_str());
      }
    }
    fprintf(out, "  }\n");

    fprintf(out, "  void Serialize(nlohmann::json *j) const {\n");
    if (!o->extends.empty()) {
      fprintf(out, "    %s::Serialize(j);\n", o->extends.c_str());
    }
    for (const auto&p : o->properties) {
      if (p.object_type == nullptr) {
        fprintf(out, "    (*j)[\"%s\"] = %s;\n",
                p.name.c_str(), p.name.c_str());
      } else {
        fprintf(out, "    %s.Serialize(&(*j)[\"%s\"]);\n",
                p.name.c_str(), p.name.c_str());
      }
    }
    fprintf(out, "  }\n");

    fprintf(out, "};\n"); // End of struct

    // functions that are picked up by the nlohmann::json serializer
    // We could generate template code once for all to_json/from_json that take
    // a T obj, but to limit method lookup confusion for other objects that
    // might interact with the json library, let's be explicit for each struct
    fprintf(out, "inline void to_json(nlohmann::json &j, const %s &obj) "
            "{ obj.Serialize(&j); }\n", o->name.c_str());
    fprintf(out, "inline void from_json(const nlohmann::json &j, %s &obj) "
            "{ obj.Deserialize(j); }\n\n", o->name.c_str());
  }

  for (auto& o : *objects) delete o;
}
