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
        "window/workDoneProgress/create", "textDocument/documentSymbol", "textDocument/semanticTokens/full",
        "$/progress", "textDocument/semanticTokens/full/delta", "textDocument/publishDiagnostics", "workspace/semanticTokens/refresh",
        "textDocument/documentLink", "textDocument/codeAction", "textDocument/signatureHelp",
        "textDocument/completion", "$/cancelRequest", "textDocument/documentHighlight",
        "textDocument/switchSourceHeader"};

      auto const & params = jsonObj["params"];
      if (method == "textDocument/hover" && !verifyType<HoverParams>(params))
        exit(1);
      else if (method == "textDocument/didOpen" && !verifyType<DidOpenParams>(params))
        exit(1);
      else if (method == "textDocument/didSave" && !verifyType<DidSaveParams>(params))
        exit(1);
      else if (method == "textDocument/didClose" && !verifyType<DidCloseParams>(params))
        exit(1);
      else if (method == "textDocument/didChange" && !verifyType<DidChangeParams>(params))
        exit(1);
      else if (method == "textDocument/clangd.fileStatus" && !verifyType<ClangdFileStatus>(params))
        exit(1);
      else if(commonMethods.contains(method))
      {
        std::cout << "no impl" << std::endl;
      }


      if (method == "textDocument/publishDiagnostics")
        std::cout << params.dump() << std::endl;
    }
    else
      std::cout << "no method" << std::endl;



    return jsonObj.dump();
  }

}
