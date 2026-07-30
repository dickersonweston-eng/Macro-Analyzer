#pragma once
#include <Geode/Result.hpp>
#include <Geode/utils/general.hpp>
#include <matjson.hpp>

using namespace geode;

namespace MsgPackDecoder {
    Result<matjson::Value> decode(ByteVector const& data);
}