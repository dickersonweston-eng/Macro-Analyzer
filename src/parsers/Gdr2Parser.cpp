#include "Gdr2Parser.hpp"
#include <Geode/utils/file.hpp>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <stdexcept>

namespace {

struct Gdr2ParseError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Gdr2Reader {
public:
    explicit Gdr2Reader(std::vector<uint8_t> const& data) : m_data(data) {}

    size_t remaining() const { return m_data.size() - m_offset; }

    std::vector<uint8_t> readBytes(size_t len) {
        if (m_offset + len > m_data.size()) {
            throw Gdr2ParseError("Unexpected end of file");
        }
        std::vector<uint8_t> bytes(m_data.begin() + m_offset, m_data.begin() + m_offset + len);
        m_offset += len;
        return bytes;
    }

    uint64_t readVarint() {
        uint64_t result = 0;
        for (int i = 0; i < 10; i++) {
            if (m_offset >= m_data.size()) {
                throw Gdr2ParseError("Unexpected end of file");
            }
            uint8_t byte = m_data[m_offset++];
            result += static_cast<uint64_t>(byte & 0x7f) << (7 * i);
            if ((byte & 0x80) == 0) break;
        }
        return result;
    }

    bool readBool() {
        return readVarint() != 0;
    }

    std::string readString() {
        auto len = readVarint();
        auto bytes = readBytes(static_cast<size_t>(len));
        return std::string(bytes.begin(), bytes.end());
    }

    double readFloat32() {
        auto bytes = readBytes(4);
        uint32_t bits = (static_cast<uint32_t>(bytes[0]) << 24)
                        | (static_cast<uint32_t>(bytes[1]) << 16)
                        | (static_cast<uint32_t>(bytes[2]) << 8)
                        | static_cast<uint32_t>(bytes[3]);
        float value;
        std::memcpy(&value, &bits, sizeof(value));
        return static_cast<double>(value);
    }

    double readFloat64() {
        auto bytes = readBytes(8);
        uint64_t bits = 0;
        for (auto b : bytes) {
            bits = (bits << 8) | b;
        }
        double value;
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

private:
    std::vector<uint8_t> const& m_data;
    size_t m_offset = 0;
};

}

Result<Replay> Gdr2Parser::parse(std::filesystem::path const& path) {
    auto readResult = geode::utils::file::readBinary(path);
    if (!readResult) {
        return Err("Failed to read file: " + readResult.unwrapErr());
    }
    auto data = readResult.unwrap();

    try {
        Gdr2Reader reader(data);

        auto magicBytes = reader.readBytes(3);
        std::string magic(magicBytes.begin(), magicBytes.end());
        if (magic != "GDR") {
            return Err("Invalid magic: \"" + magic + "\"");
        }

        auto version = reader.readVarint();
        (void)version;
        auto inputTag = reader.readString();

        auto author = reader.readString();
        auto description = reader.readString();
        auto duration = reader.readFloat32();
        auto gameVersion = reader.readVarint();
        auto framerate = reader.readFloat64();
        auto seed = reader.readVarint();
        auto coins = reader.readVarint();
        auto ldm = reader.readBool();
        auto platformer = reader.readBool();
        auto botName = reader.readString();
        auto botVersion = reader.readVarint();
        auto levelId = reader.readVarint();
        auto levelName = reader.readString();

        auto extensionSize = reader.readVarint();
        reader.readBytes(static_cast<size_t>(extensionSize)); // No Known Generic Schema, Skip

        auto deathCount = reader.readVarint();
        std::vector<int> deaths;
        uint64_t deathAcc = 0;
        for (uint64_t i = 0; i < deathCount; i ++) {
            deathAcc += reader.readVarint();
            deaths.push_back(static_cast<int>(deathAcc));
        }

        reader.readVarint(); // Total Input Count (Not Used To Bound The Read Loop)
        auto p1Remaining = reader.readVarint();
        bool hasInputExt = !inputTag.empty();

        std::vector<InputFrame> frames;
        uint64_t p = 0;
        while (reader.remaining() > 0) {
            auto packed = reader.readVarint();

            uint64_t delta;
            int button;
            bool down;
            if (platformer) {
                delta = packed >> 3;
                button = static_cast<int>((packed >> 1) & 3);
                down = (packed & 1) != 0;
            } else {
                delta = packed >> 1;
                button = 1;
                down = (packed & 1) != 0;
            }

            auto frameNum = delta + p;
            bool player2 = p1Remaining == 0;

            if (hasInputExt) {
                auto extSize = reader.readVarint();
                reader.readBytes(static_cast<size_t>(extSize));
            }

            InputFrame frame;
            frame.frameNum = static_cast<int>(frameNum);
            frame.timeStampMs = (static_cast<double>(frameNum) / framerate) * 1000;
            frame.button = button;
            frame.down = down;
            frame.player2 = player2;
            frames.push_back(frame);

            p = frameNum;
            if (p1Remaining > 0) {
                p1Remaining--;
                if (p1Remaining == 0) p = 0;
            }
        }
        std::sort(frames.begin(), frames.end(), [](InputFrame const& a, InputFrame const& b) {
            return a.frameNum < b.frameNum;
        });

        Replay replay;
        replay.fileName = path.filename().string();
        replay.fileFormat = ReplayFileFormat::Gdr2;
        replay.author = author.empty() ? "Unknown" : author;
        replay.botName = botName.empty() ? path.stem().string() : botName;
        replay.botVersion = std::to_string(botVersion);
        replay.description = description;
        replay.levelName = levelName;
        replay.levelId = static_cast<int>(levelId);
        replay.duration = duration * 1000;
        replay.frameRate = framerate;
        replay.coins = static_cast<int>(coins);
        replay.seed = static_cast<int>(seed);
        replay.ldm = ldm;
        replay.platformer = platformer;
        replay.gameVersion = static_cast<double>(gameVersion);
        replay.deaths = deaths;
        replay.frames = frames;

        return Ok(replay);
    } catch (Gdr2ParseError const& e) {
        return Err(std::string(e.what()));
    }
}