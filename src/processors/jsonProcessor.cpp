#include "jsonProcessor.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <set>

#include "../json/messageTypes.hpp"

namespace lsp_proxy {

  std::string JSONProcessor::process(std::string const & message)
  {
    using namespace nlohmann;
    json jsonObj = json::parse(message);

    if (jsonObj.contains("method"))
    {
      std::string method = jsonObj["method"];
      method.erase(remove(method.begin(), method.end(), '\"'), method.end());
      std::cout << method << std::endl;
      std::set<std::string> commonMethods = {"initialize", "initialized", "$/setTrace",
        "window/workDoneProgress/create", "textDocument/clangd.fileStatus", "textDocument/documentSymbol", "textDocument/semanticTokens/full",
        "$/progress", "textDocument/semanticTokens/full/delta", "textDocument/publishDiagnostics", "workspace/semanticTokens/refresh",
        "textDocument/documentLink", "textDocument/codeAction", "textDocument/signatureHelp",
        "textDocument/completion", "$/cancelRequest", "textDocument/documentHighlight",
        "textDocument/switchSourceHeader"};

      if (method == "textDocument/hover")
        HoverParams params = jsonObj["params"];
      else if (method == "textDocument/didOpen")
        DidOpenParams params = jsonObj["params"];
      else if (method == "textDocument/didSave")
        DidSaveParams params = jsonObj["params"];
      else if (method == "textDocument/didClose")
        DidCloseParams params = jsonObj["params"];
      else if (method == "textDocument/didChange")
      {
        std::cout << jsonObj["params"].dump() << std::endl;
        DidChangeParams params = jsonObj["params"];
        json apa;
        to_json(apa, params);
        std::cout << apa.dump() << std::endl;
        std::cout << params.contentChanges.size() << std::endl;
      }
      else if(commonMethods.contains(method))
      {
        std::cout << "no impl" << std::endl;
      }
      else
      {
        std::cout << jsonObj.dump() << std::endl;
      }
    }
    else
      std::cout << "no method" << std::endl;


    return jsonObj.dump();
  }

}
