#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace lsp_proxy {

  struct Position
  {
    uint line;
    uint character;
  };

  inline void to_json(nlohmann::json & j, const Position & o) {
    j = nlohmann::json({{"line", o.line}, {"character", o.character}});
  }

  inline void from_json(const nlohmann::json & j, Position & o) {
    j.at("line").get_to(o.line);
    j.at("character").get_to(o.character);
  }

  struct TextDocument
  {
    std::string uri;
  };

  inline void to_json(nlohmann::json & j, const TextDocument & o) {
    j = nlohmann::json({{"uri", o.uri}});
  }

  inline void from_json(const nlohmann::json & j, TextDocument & o) {
    j.at("uri").get_to(o.uri);
  }

  struct TextDocumentItem
  {
    std::string uri;
    std::string languageId;
    int version;
    std::string text;
  };

  inline void to_json(nlohmann::json & j, const TextDocumentItem & o) {
    j = nlohmann::json({{"uri", o.uri}});
    j = nlohmann::json({{"languageId", o.languageId}});
    j = nlohmann::json({{"version", o.version}});
    j = nlohmann::json({{"text", o.text}});
  }

  inline void from_json(const nlohmann::json & j, TextDocumentItem & o) {
    j.at("uri").get_to(o.uri);
    j.at("languageId").get_to(o.languageId);
    j.at("version").get_to(o.version);
    j.at("text").get_to(o.text);
  }
}