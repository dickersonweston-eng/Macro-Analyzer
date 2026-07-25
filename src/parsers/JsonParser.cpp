#include "JsonParser.hpp"
#include <Geode/utils/file.hpp>
#include <matjson.hpp>
#include <algorithm>

using namespace geode::prelude;

namespace {
    std::string getString(matjson::Value const& obj, std::string_view key, std::string const& def) {
        if (!obj.contains(key)) return def;
        return obj[key].asString().unwrapOr(def);
    }

    int getInt(matjson::Value const& obj, std::string_view key, int def) {
        if (!obj.contains(key)) return def;
        return static_cast<int>(obj[key].asInt().unwrapOr(def));
    }

    double getDouble(matjson::Value const& obj, std::string_view key, double def) {
        if (!obj.contains(key)) return def;
        return obj[key].asDouble().unwrapOr(def);
    }
    bool getBool(matjson::Value const& obj, std::string_view key, bool def) {
        if (!obj.contains(key)) return def;
        return obj[key].asBool().unwrapOr(def);
    }
}

Result<Replay> JsonParser::parse(std::filesystem::path const& path) {
    auto textResult = file::readString(path);
    if (!textResult) return Err("Failed To Read File: " + textResult.unwrapErr());

    auto jsonResult = matjson::parse(textResult.unwrap());
    if (!jsonResult) return Err("Failed To Parse JSON: " + std::string(jsonResult.unwrapErr()));

    auto root = jsonResult.unwrap();

    Replay replay;
    replay.fileName = path.filename().string();
    replay.fileFormat = ReplayFileFormat::Json;

    replay.author = getString(root, "author", "Unknown");

    if (root.contains("bot") && root["bot"].isObject()) {
        replay.botName = getString(root["bot"], "name", "Unknown Bot");
        replay.botVersion = getString(root["bot"], "version", "Unknown");
    } else {
        replay.botName = getString(root, "botName", "Unknown Bot");
        replay.botVersion = getString(root, "botVersion", "Unknown");
    }

    replay.description = getString(root, "description", "");
    replay.coins = getInt(root, "coins", 0);
    replay.seed = getInt(root, "seed", 0);
    replay.ldm = getBool(root, "ldm", false);
    replay.gameVersion = getDouble(root, "gameVersion", 0);
    replay.frameRate = getDouble(root, "framerate", 240);

    if (root.contains("level") && root["level"].isObject()) {
        replay.levelId = getInt(root["level"], "id", 0);
        replay.levelName = getString(root["level"], "name", "");
    }

    bool hasFramerateField = root.contains("framerate");
    double rawDuration = getDouble(root, "duration", 0);

    std::vector<InputFrame> frames;
    if (root.contains("inputs") && root["inputs"].isArray()) {
        for (auto& entry : root["inputs"]) {
            InputFrame frame;
            frame.frameNum = getInt(entry, "frame", 0);
            frame.button = getInt(entry, "btn", 1);
            frame.down = getBool(entry, "down", false);
            frame.player2 = getBool(entry, "2p", false);
            frames.push_back(frame);
        }
    }

    std::sort(frames.begin(), frames.end(), [](InputFrame const& a, InputFrame const& b) {
        return a.frameNum < b.frameNum;
    });

    int maxFrame = frames.empty() ? 0 : frames.back().frameNum;

    double durationSeconds;
    if (!hasFramerateField && rawDuration > maxFrame * 0.5) {
        durationSeconds = rawDuration / replay.frameRate;
    } else {
        durationSeconds = rawDuration;
    }
    replay.duration = durationSeconds * 1000.0;

    for (auto& frame : frames) {
        frame.timeStampMs = (frame.frameNum / replay.frameRate) * 1000.0;
    }
    replay.frames = std::move(frames);

    return Ok(replay);
}