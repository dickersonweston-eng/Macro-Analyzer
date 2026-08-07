#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/SliderNode.hpp>
#include "../../../Replay.hpp"

using namespace geode::prelude;

class PlayerTab : public CCNode {
protected:
    bool init(float width, float height);
    void update(float dt) override;
    void onTogglePlay(CCObject* sender);
    void onSlide(SliderNode* sender, float value);
    void updateTimeLabel();
    void updateButtonIndicators();

    Replay m_replay;
    bool m_hasReplay = false;
    bool m_playing = false;
    double m_currentTimeMs = 0;

    SliderNode* m_scrubber = nullptr;
    CCLabelBMFont* m_timeLabel = nullptr;
    CCMenuItemSpriteExtra* m_playBtn = nullptr;
    CCLayerColor* m_jumpIndicator = nullptr;
    CCLayerColor* m_leftIndicator = nullptr;
    CCLayerColor* m_rightIndicator = nullptr;

public:
    static PlayerTab* create(float width, float height);
    void populate(Replay const& replay);
};