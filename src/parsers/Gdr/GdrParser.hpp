#pragma once
#include <Geode/Result.hpp>
#include <filesystem>
#include "../../Replay.hpp"

using namespace geode;

namespace GdrParser {
    Result<Replay> parse(std::filesystem::path const& path);
}