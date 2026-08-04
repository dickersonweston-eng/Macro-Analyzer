#include "AnalysisTab.hpp"
#include <fmt/format.h>

CCNode* AnalysisTab::createMetricCard(std::string const& label, std::string const& value, ccColor3B color, float delay) {
    auto card = CCNodeRGBA::create();
    card->setContentSize({108.f, 46.f});
    card->setCascadeOpacityEnabled(true);
    card->setAnchorPoint({0.5f, 0.5f});

    auto panel = NineSlice::create("GJ_square01.png");
    panel->setContentSize(card->getContentSize());
    panel->setPosition(card->getContentSize() / 2);
    card->addChild(panel);

    auto labelText = CCLabelBMFont::create(label.c_str(), "chatFont.fnt");
    labelText->setScale(0.35f);
    labelText->setAnchorPoint({0.f, 1.f});
    labelText->setPosition(10.f, card->getContentSize().height - 8.f);
    card->addChild(labelText);

    auto valueText = CCLabelBMFont::create(value.c_str(), "chatFont.fnt");
    valueText->limitLabelWidth(88.f, 0.5f, 0.25f);
    valueText->setColor(color);
    valueText->setAnchorPoint({0.f, 1.f});
    valueText->setPosition(10.f, card->getContentSize().height - 24.f);
    card->addChild(valueText);

    card->setOpacity(0);
    card->runAction(
        CCSequence::create(
            CCDelayTime::create(delay),
            CCFadeTo::create(0.3, 255),
            nullptr
        )
    );

    return card;
}

void AnalysisTab::populate(Replay const& replay) {
    this->removeAllChildren();

    double seconds = replay.duration / 1000.0;
    double density = seconds > 0 ? replay.frames.size() / seconds : 0.0;

    auto card = createMetricCard("Input Density", fmt::format("{:.2f}/s", density), {140, 255, 140}, 0.f);
    card->setPosition(this->getContentSize().width / 2.f, this->getContentSize().height - 100.f);
    this->addChild(card);
}

bool AnalysisTab::init(float width, float height) {
    if (!CCNode::init()) return false;
    this->setContentSize({width, height});
    return true;
}

AnalysisTab* AnalysisTab::create(float width, float height) {
    auto ret = new AnalysisTab();
    if (ret->init(width, height)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}