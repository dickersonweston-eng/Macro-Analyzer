#include "MsgPackDecoder.hpp"
#include <cstring>
#include <stdexcept>

namespace {

struct MsgPackParseError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

class MsgPackReader {
public:
    explicit MsgPackReader(ByteVector const& data) : m_data(data) {}

    uint8_t readU8() {
        if (m_offset >= m_data.size()) throw MsgPackParseError("Unexpected End Of File");
        return m_data[m_offset++];
    }

    std::vector<uint8_t> readBytes(size_t len) {
        if (m_offset + len > m_data.size()) throw MsgPackParseError("Unexpected End Of File");
        std::vector<uint8_t> bytes(m_data.begin() + m_offset, m_data.begin() + m_offset + len);
        m_offset += len;
        return bytes;
    }

    uint64_t readBigEndian(size_t len) {
        auto bytes = readBytes(len);
        uint64_t value = 0;
        for (auto b : bytes) value = (value << 8) | b;
        return value;
    }

    double readFloat32() {
        uint32_t bits = static_cast<uint32_t>(readBigEndian(4));
        float value;
        std::memcpy(&value, &bits, sizeof(value));
        return static_cast<double>(value);
    }

    double readFloat64() {
        uint64_t bits = readBigEndian(8);
        double value;
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

    matjson::Value readValue() {
        uint8_t tag = readU8();

        if (tag <= 0x7f) return matjson::Value(static_cast<std::intmax_t>(tag));
        if (tag >= 0x80 && tag <= 0x8f) return readMap(tag & 0x0f);
        if (tag >= 0x90 && tag <= 0x9f) return readArray(tag & 0x0f);
        if (tag >= 0xa0 && tag <= 0xbf) return readString(tag & 0x1f);
        if (tag >= 0xe0) return matjson::Value(static_cast<std::intmax_t>(static_cast<int8_t>(tag)));

        switch (tag) {
            case 0xc0: return matjson::Value(nullptr);
            case 0xc2: return matjson::Value(false);
            case 0xc3: return matjson::Value(true);
            case 0xc4: return readString(readBigEndian(1));
            case 0xc5: return readString(readBigEndian(2));
            case 0xc6: return readString(readBigEndian(4));
            case 0xca: return matjson::Value(readFloat32());
            case 0xcb: return matjson::Value(readFloat64());
            case 0xcc: return matjson::Value(static_cast<std::uintmax_t>(readBigEndian(1)));
            case 0xcd: return matjson::Value(static_cast<std::uintmax_t>(readBigEndian(2)));
            case 0xce: return matjson::Value(static_cast<std::uintmax_t>(readBigEndian(4)));
            case 0xcf: return matjson::Value(static_cast<std::uintmax_t>(readBigEndian(8)));
            case 0xd0: return matjson::Value(static_cast<std::intmax_t>(static_cast<int8_t>(readBigEndian(1))));
            case 0xd1: return matjson::Value(static_cast<std::intmax_t>(static_cast<int16_t>(readBigEndian(2))));
            case 0xd2: return matjson::Value(static_cast<std::intmax_t>(static_cast<int32_t>(readBigEndian(4))));
            case 0xd3: return matjson::Value(static_cast<std::intmax_t>(static_cast<int64_t>(readBigEndian(8))));
            case 0xd9: return readString(readBigEndian(1));
            case 0xda: return readString(readBigEndian(2));
            case 0xdb: return readString(readBigEndian(4));
            case 0xdc: return readArray(readBigEndian(2));
            case 0xdd: return readArray(readBigEndian(4));
            case 0xde: return readMap(readBigEndian(2));
            case 0xdf: return readMap(readBigEndian(4));
            default:
                throw MsgPackParseError("Unsupported Msgpack Tag: " + std::to_string(tag));
        }
    }

private:
    matjson::Value readString(size_t len) {
        auto bytes = readBytes(len);
        return matjson::Value(std::string(bytes.begin(), bytes.end()));
    }

    matjson::Value readArray(size_t count) {
        auto arr = matjson::Value::array();
        for (size_t i = 0; i < count; i++) {
            arr.push(readValue());
        }
        return arr;
    }

    matjson::Value readMap(size_t count) {
        auto obj = matjson::Value::object();
        for (size_t i = 0; i < count; i++) {
            auto key = readValue();
            auto value = readValue();
            obj.set(key.asString().unwrapOr(""), value);
        }
        return obj;
    }

    ByteVector const& m_data;
    size_t m_offset = 0;
};

}

Result<matjson::Value> MsgPackDecoder::decode(ByteVector const& data) {
    try {
        MsgPackReader reader(data);
        return Ok(reader.readValue());
    } catch (MsgPackParseError const& e) {
        return Err(std::string(e.what()));
    }
}