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
        "$/progress", "textDocument/semanticTokens/full/delta", "workspace/semanticTokens/refresh",
        "textDocument/documentLink", "textDocument/codeAction", "textDocument/signatureHelp",
        "textDocument/completion", "$/cancelRequest", "textDocument/documentHighlight",
        "textDocument/switchSourceHeader"};

      auto const & params = jsonObj["params"];
      if (method == "textDocument/hover")
        assertType<HoverParams>(params);
      else if (method == "textDocument/didOpen")
        assertType<DidOpenParams>(params);
      else if (method == "textDocument/didSave")
        assertType<DidSaveParams>(params);
      else if (method == "textDocument/didClose")
        assertType<DidCloseParams>(params);
      else if (method == "textDocument/didChange")
        assertType<DidChangeParams>(params);
      else if (method == "textDocument/clangd.fileStatus")
        assertType<ClangdFileStatus>(params);
      else if (method == "textDocument/publishDiagnostics")
        assertType<PublishDiagnosticsParams>(params);
      else if(commonMethods.contains(method))
      {
        std::cout << "no impl" << std::endl;
      }


      // if (method == "textDocument/publishDiagnostics")
      //   std::cout << params.dump() << std::endl;
    }
    else
      std::cout << "no method" << std::endl;



    return jsonObj.dump();
  }

}
