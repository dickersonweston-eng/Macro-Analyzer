#pragma once
#include <string>
#include <vector>

enum class ReplayFileFormat {
    Gdr2, Gdr, Json
};

struct InputFrame {
    int frameNum;
    double timeStampMs;
    int button; // 1 = Jump, 2 = Left, 3 = Right
    bool down;
    bool player2;
};

struct Replay {
    std::string fileName;
    ReplayFileFormat fileFormat;
    std::string author = "Unknown";
    std::string botName = "Unknown Bot";
    std::string botVersion = "Unknown";
    std::string description;
    std::string levelName;
    int levelId = 0;
    double duration = 0; // Milliseconds
    double frameRate = 240;
    int coins = 0;
    int seed = 0;
    bool ldm = false;
    bool platformer = false;
    double gameVersion = 0;
    std::vector<int> deaths;
    std::vector<InputFrame> frames;
};