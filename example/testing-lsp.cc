
#include "lsp-protocol.h"
#include <iostream>

int main() {
  Range range;
  range.start.line = 5;
  range.start.character = 17;
  range.end.line = 5;
  range.end.character = 32;

  nlohmann::json j = range;
  std::cout << j << "\n";

  // Test round-trip
  Range re_generated = j.get<Range>();

  nlohmann::json j2 = re_generated;
  std::cout << j2 << "\n";

  RequestMessage msg;
  msg.id = "x";
  msg.method = "testcall";
  msg.params = range;   // Assigning to an object type.

  nlohmann::json j3 = msg;
  std::cout << j3 << "\n";
}
