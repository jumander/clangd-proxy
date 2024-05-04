#include "jsonProcessor.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

namespace lsp_proxy {

  std::string JSONProcessor::process(std::string const & message)
  {
    using namespace nlohmann;
    json jsonObj = json::parse(message);
    return jsonObj.dump();
  }

}