#include <iostream>
#include <iomanip>

#include "textdocs.h"

int main() {
  std::cout << "-- Simple round-trip deserialize/serialize --" << std::endl;
  nlohmann::json in_json = { { "url", "https://github.com/hzeller/jcxxgen"},
                             { "bytes", 12345 } };
  TextDocument td = in_json;  // Automatic conversion from json to object
  td.bytes = 42;  // Let's change the value to see that it is serialized

  nlohmann::json out_json = td;   // Automatic conversion back to json.
  std::cout << std::setw(2) << out_json << std::endl;

  std::cout << "-- Anytype object and repeated fields --" << std::endl;
  ReadingList reading_list;
  reading_list.other = nlohmann::json({{ "freeform", "value"}});
  reading_list.to_read.emplace_back(td);
  TextDocument other;
  other.url = "https://timg.sh";
  other.bytes = 9876;
  reading_list.to_read.emplace_back(other);

  out_json = reading_list;
  std::cout << std::setw(2) << out_json << std::endl;

  std::cout << "-- Optional fields --" << std::endl;
  OptionallyVersionedTextDocument versioned;
  versioned.url = "http://timg.sh/";
  versioned.bytes = 12345;
  versioned.version = 1;

  // We need to tell if we want the version field included
  versioned.has_version = true;

  out_json = versioned;
  std::cout << std::setw(2) << out_json << std::endl;

  // If `has_version` is set to false the `version` will not be serialized
  versioned.has_version = false;
  out_json = versioned;
  std::cout << std::setw(2) << out_json << std::endl;
}
