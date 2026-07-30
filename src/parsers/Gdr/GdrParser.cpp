#include "GdrParser.hpp"
#include "../Core/SchemaParser.hpp"
#include "../Core/MsgPackDecoder.hpp"
#include <Geode/utils/file.hpp>

using namespace geode::prelude;

Result<Replay> GdrParser::parse(std::filesystem::path const& path) {
    auto readResult = file::readBinary(path);
    if (!readResult) return Err("Failed To Read File: " + readResult.unwrapErr());

    auto decodeResult = MsgPackDecoder::decode(readResult.unwrap());
    if (!decodeResult) return Err("Failed To Decode Msgpack: " + decodeResult.unwrapErr());

    auto root = decodeResult.unwrap();
    return Ok(SchemaParser::makeReplay(root, ReplayFileFormat::Gdr, path.filename().string()));
}