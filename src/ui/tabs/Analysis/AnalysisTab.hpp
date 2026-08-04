#pragma once
#include <Geode/Geode.hpp>
#include "../../../Replay.hpp"

using namespace geode::prelude;

class AnalysisTab : public CCNode {
protected:
    bool init(float width, float height);
    CCNode* createMetricCard(std::string const& label, std::string const& value, ccColor3B color, float delay);

public:
    static AnalysisTab* create(float width, float height);
    void populate(Replay const& replay);
};