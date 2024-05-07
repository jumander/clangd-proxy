#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "baseTypes.hpp"

namespace lsp_proxy {

  struct HoverParams
  {
    TextDocument textDocument;
    Position position;
  };

  inline void to_json(nlohmann::json & j, const HoverParams & o) {
    j = nlohmann::json({{"textDocument", o.textDocument}, {"position", o.position}});
  }

  inline void from_json(const nlohmann::json & j, HoverParams & o) {
    j.at("textDocument").get_to(o.textDocument);
    j.at("position").get_to(o.position);
  }

  struct DidOpenParams
  {
    TextDocumentItem textDocument;
  };

  inline void to_json(nlohmann::json & j, const DidOpenParams & o) {
    j = nlohmann::json({{"textDocument", o.textDocument}});
  }

  inline void from_json(const nlohmann::json & j, DidOpenParams & o) {
    j.at("textDocument").get_to(o.textDocument);
  }
}