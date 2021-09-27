
#include "lsp-protocol.h"
#include <iostream>

template <typename T> static void PrintAsJson(const T &o) {
  nlohmann::json j = o;
  std::cout << j << "\n";
}

int main() {
  Range range;
  range.start.line = 5;
  range.start.character = 17;
  range.end.line = 5;
  range.end.character = 32;

  nlohmann::json range_json = range;     // Automatic serialization with assign
  std::cout << range_json << "\n";

  PrintAsJson(range_json.get<Range>());  // Round-trip back to object

  std::vector<Range> range_list;
  range_list.push_back(range);
  range_list.push_back(range);

  RequestMessage msg;
  msg.id = "42";
  msg.method = "testcall";
  msg.params = range_list;  // automatic conversion to the proper array type.

  nlohmann::json msg_json = msg;        // Serializate to json
  std::cout << msg_json << "\n";

  // Round-trip: convert json back to RequestMessage
  const RequestMessage msg2 = msg_json.get<RequestMessage>();

  // Extract back an c++ vector from the raw parameter json.
  std::vector<Range> range_list2 = msg2.params.get<std::vector<Range>>();

  PrintAsJson(msg2);
}
