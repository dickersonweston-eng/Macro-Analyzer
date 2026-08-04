#pragma once
#include <Geode/Geode.hpp>
#include "../../../Replay.hpp"

using namespace geode::prelude;

class StatsTab : public CCNode {
protected:
    bool init(float width, float height);
    CCNode* createMetricCard(std::string const& label, std::string const& value, ccColor3B color, float delay);

public:
    static StatsTab* create(float width, float height);
    void populate(Replay const& replay);
};