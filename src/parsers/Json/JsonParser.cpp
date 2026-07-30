#include "JsonParser.hpp"
#include "../Core/SchemaParser.hpp"
#include <Geode/utils/file.hpp>
#include <matjson.hpp>

using namespace geode::prelude;

Result<Replay> JsonParser::parse(std::filesystem::path const& path) {
    auto textResult = file::readString(path);
    if (!textResult) return Err("Failed To Read File: " + textResult.unwrapErr());

    auto jsonResult = matjson::parse(textResult.unwrap());
    if (!jsonResult) return Err("Failed To Parse JSON: " + std::string(jsonResult.unwrapErr()));

    auto root = jsonResult.unwrap();
    return Ok(SchemaParser::makeReplay(root, ReplayFileFormat::Json, path.filename().string()));
}