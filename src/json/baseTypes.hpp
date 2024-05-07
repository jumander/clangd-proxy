#pragma once
#include <string>
#include <optional>
#include <nlohmann/json.hpp>

namespace lsp_proxy {

  template <typename T>
  bool verifyType(const nlohmann::json & before)
  {
    T type = before;
    nlohmann::json after;
    to_json(after, type);
    return before.dump() == after.dump();
  }
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

  struct Range
  {
    Position start;
    Position end;
  };

  inline void to_json(nlohmann::json & j, const Range & o) {
    j = nlohmann::json({{"start", o.start}, {"end", o.end}});
  }

  inline void from_json(const nlohmann::json & j, Range & o) {
    j.at("start").get_to(o.start);
    j.at("end").get_to(o.end);
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

  struct VersionedTextDocument
  {
    std::string uri;
    int version;
  };

  inline void to_json(nlohmann::json & j, const VersionedTextDocument & o) {
    j = nlohmann::json({{"uri", o.uri}, {"version", o.version}});
  }

  inline void from_json(const nlohmann::json & j, VersionedTextDocument & o) {
    j.at("uri").get_to(o.uri);
    j.at("version").get_to(o.version);
  }

  struct TextDocumentItem
  {
    std::string uri;
    std::string languageId;
    int version;
    std::string text;
  };

  inline void to_json(nlohmann::json & j, const TextDocumentItem & o) {
    j = nlohmann::json({{"uri", o.uri}, {"languageId", o.languageId}, {"version", o.version}, {"text", o.text}});
  }

  inline void from_json(const nlohmann::json & j, TextDocumentItem & o) {
    j.at("uri").get_to(o.uri);
    j.at("languageId").get_to(o.languageId);
    j.at("version").get_to(o.version);
    j.at("text").get_to(o.text);
  }

  struct TextDocumentContentChangeEvent
  {
    std::optional<Range> range;
    std::optional<uint> rangeLength;
    std::string text;
  };

  inline void to_json(nlohmann::json & j, const TextDocumentContentChangeEvent & o) {
    j = nlohmann::json({{"text", o.text}});
    if (o.range)
      j["range"] = *o.range;
    if (o.rangeLength)
      j["rangeLength"] = *o.rangeLength;
  }

  inline void from_json(const nlohmann::json & j, TextDocumentContentChangeEvent & o) {
    o.range = j.contains("range") ? j.at("range").get<Range>() : std::optional<Range>{};
    o.rangeLength = j.contains("rangeLength") ? j.at("rangeLength").get<uint>() : std::optional<uint>{};
    j.at("text").get_to(o.text);
  }

}