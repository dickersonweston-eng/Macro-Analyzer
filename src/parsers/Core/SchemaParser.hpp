#pragma once
#include <matjson.hpp>
#include "../../Replay.hpp"

// Shared field-mapping logic for .json and .gdr replay files — same schema, different encoding (text vs msgpack)

namespace SchemaParser {
    Replay makeReplay(matjson::Value const& root, ReplayFileFormat format, std::string const& fileName);
}