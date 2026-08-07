#include "PlayerTab.hpp"
#include <fmt/format.h>

bool PlayerTab::init(float width, float height) {
    if (!CCNode::init()) return false;
    this->setContentSize({width, height});
    this->scheduleUpdate();

    m_scrubber = SliderNode::create([this](SliderNode* sender, float value) {
        onSlide(sender, value);
    });
    m_scrubber->setContentSize({width - 90.f, 12.f});
    m_scrubber->setPosition(width / 2.f, height - 40.f);
    this->addChild(m_scrubber);
    m_scrubber->setVisible(false);

    m_timeLabel = CCLabelBMFont::create("0.0s / 0.0s", "chatFont.fnt");
    m_timeLabel->setScale(0.4f);
    m_timeLabel->setPosition(width / 2.f, height - 60.f);
    this->addChild(m_timeLabel);
    m_timeLabel->setVisible(false);

    auto menu = CCMenu::create();
    menu->setPosition(0.f, 0.f);

    auto playLabel = CCLabelBMFont::create("Play", "chatFont.fnt");
    playLabel->setScale(0.5f);
    m_playBtn = CCMenuItemSpriteExtra::create(
        playLabel, this, menu_selector(PlayerTab::onTogglePlay)
    );
    m_playBtn->setPosition(width / 2.f, height - 90.f);
    menu->addChild(m_playBtn);
    m_playBtn->setVisible(false);

    this->addChild(menu);

    float indicatorY = height - 140.f;
    float spacing = 70.f;

    m_jumpIndicator = CCLayerColor::create(ccc4(60, 60, 65, 255), 40.f, 40.f);
    m_jumpIndicator->setAnchorPoint({0.5f, 0.5f});
    m_jumpIndicator->setPosition(width / 2.f - spacing, indicatorY);
    this->addChild(m_jumpIndicator);
    m_jumpIndicator->setVisible(false);

    auto jumpLabel = CCLabelBMFont::create("Jump", "chatFont.fnt");
    jumpLabel->setScale(0.35f);
    jumpLabel->setPosition(width / 2.f - spacing, indicatorY - 30.f);
    this->addChild(jumpLabel);

    m_rightIndicator = CCLayerColor::create(ccc4(60, 60, 65, 255), 40.f, 40.f);
    m_rightIndicator->setAnchorPoint({0.5f, 0.5f});
    m_rightIndicator->setPosition(width / 2.f + spacing, indicatorY);
    this->addChild(m_rightIndicator);
    m_rightIndicator->setVisible(false);

    auto rightLabel = CCLabelBMFont::create("Right", "chatFont.fnt");
    rightLabel->setScale(0.35f);
    rightLabel->setPosition(width / 2.f + spacing, indicatorY - 30.f);
    this->addChild(rightLabel);

    m_leftIndicator = CCLayerColor::create(ccc4(60, 60, 65, 255), 40.f, 40.f);
    m_leftIndicator->setAnchorPoint({0.5f, 0.5f});
    m_leftIndicator->setPosition(width / 2.f, indicatorY);
    this->addChild(m_leftIndicator);
    m_leftIndicator->setVisible(false);

    auto leftLabel = CCLabelBMFont::create("Left", "chatFont.fnt");
    leftLabel->setScale(0.35);
    leftLabel->setPosition(width / 2.f, indicatorY - 30.f);
    this->addChild(leftLabel);

    return true;
}

PlayerTab* PlayerTab::create(float width, float height) {
    auto ret = new PlayerTab();
    if (ret->init(width, height)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void PlayerTab::populate(Replay const& replay) {
    m_replay = replay;
    m_hasReplay = true;
    m_playing = false;
    m_currentTimeMs = 0;

    m_scrubber->setVisible(true);
    m_timeLabel->setVisible(true);
    m_playBtn->setVisible(true);
    m_jumpIndicator->setVisible(true);
    m_leftIndicator->setVisible(true);
    m_rightIndicator->setVisible(true);
    m_scrubber->setMin(0.f);
    m_scrubber->setMax((float)replay.duration);
    m_scrubber->setValue(0.f);

    updateTimeLabel();
    updateButtonIndicators();
}

void PlayerTab::update(float dt) {
    if (!m_hasReplay || !m_playing) return;

    m_currentTimeMs += dt * 1000.0;
    if (m_currentTimeMs >= m_replay.duration) {
        m_currentTimeMs = m_replay.duration;
        m_playing = false;
        m_playBtn->setNormalImage(CCLabelBMFont::create("Play", "chatFont.fnt"));
    }

    m_scrubber->setValue((float)m_currentTimeMs);
    updateTimeLabel();
    updateButtonIndicators();
}

void PlayerTab::onTogglePlay(CCObject* sender) {
    if (!m_hasReplay) return;

    m_playing = !m_playing;
    if (m_playing && m_currentTimeMs >= m_replay.duration) {
        m_currentTimeMs = 0;
    }

    auto label = CCLabelBMFont::create(m_playing ? "Pause" : "Play", "chatFont.fnt");
    label->setScale(0.5f);
    m_playBtn->setNormalImage(label);
}

void PlayerTab::onSlide(SliderNode* sender, float value) {
    m_currentTimeMs = value;
    updateTimeLabel();
    updateButtonIndicators();
}

void PlayerTab::updateTimeLabel() {
    if (!m_hasReplay) return;
    m_timeLabel->setString(
        fmt::format("{:.1f}s / {:.1f}s", m_currentTimeMs / 1000.0, m_replay.duration / 1000.0).c_str()
    );
}

void PlayerTab::updateButtonIndicators() {
    if (!m_hasReplay) return;

    bool jump = false, left = false, right = false;
    for (auto& frame : m_replay.frames) {
        if (frame.timeStampMs > m_currentTimeMs) break;
        if (frame.player2) continue;
        if (frame.button == 1) jump = frame.down;
        else if (frame.button == 2) left = frame.down;
        else if (frame.button == 3) right = frame.down;
    }

    m_jumpIndicator->setColor(jump ? ccColor3B{140, 255, 140} : ccColor3B{60, 60, 65});
    m_leftIndicator->setColor(left ? ccColor3B{120, 230, 255} : ccColor3B{60, 60, 65});
    m_rightIndicator->setColor(right ? ccColor3B{255, 140, 255} : ccColor3B{60, 60, 65});
}