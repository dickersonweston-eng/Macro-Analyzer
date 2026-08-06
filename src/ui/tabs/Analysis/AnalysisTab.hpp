#pragma once
#include <Geode/Geode.hpp>
#include "../../../Replay.hpp"

using namespace geode::prelude;

class AnalysisTab : public CCNode {
protected:
    bool init(float width, float height);
    CCNode* createInputTypeChart(Replay const& replay, float width, float height);
    CCNode* createInputsOverTimeChart(Replay const& replay, float width, float height);

public:
    static AnalysisTab* create(float width, float height);
    void populate(Replay const& replay);
};