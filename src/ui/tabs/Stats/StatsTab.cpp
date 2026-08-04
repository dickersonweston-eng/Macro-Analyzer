#include "StatsTab.hpp"
#include <fmt/format.h>

CCNode* StatsTab::createMetricCard(std::string const& label, std::string const& value, ccColor3B color, float delay) {
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

void StatsTab::populate(Replay const& replay) {
    this->removeAllChildren();

    struct Metric { std::string label; std::string value; ccColor3B color; };
    std::vector<Metric> metrics = {
        { "Author", replay.author, {140, 255, 140} },
        { "Bot", replay.botName + " v" + replay.botVersion, {140, 255, 140} },
        { "Duration", fmt::format("{:.1f}s", replay.duration / 1000.0), {140, 255, 140} },
        { "Framerate", fmt::format("{} fps", (int)replay.frameRate), {120, 230, 255} },
        { "Inputs", std::to_string(replay.frames.size()), {140, 255, 140} },
        { "Coins", std::to_string(replay.coins), {140, 255, 140} },
        { "Seed", std::to_string(replay.seed), {140, 255, 140} },
        { "Mode", replay.platformer ? "Platformer" : "Classic", {255, 140, 255} },
        { "LDM", replay.ldm ? "Yes" : "No", {255, 140, 255} },
        { "Level ID", std::to_string(replay.levelId), {140, 255, 140} },
        { "Level", replay.levelName, {140, 255, 140} },
        { "Format", [&]() -> std::string {
            if (replay.fileFormat == ReplayFileFormat::Json) return "JSON";
            if (replay.fileFormat == ReplayFileFormat::Gdr) return "GDR";
            return "GDR2";
        }(), {140, 255, 140} },
    };

    int cols = 3;
    float cardW = 108.f, cardH = 46.f, gapX = 12.f, gapY = 8.f;
    float gridW = cols * cardW + (cols - 1) * gapX;
    float startX = (this->getContentSize().width - gridW) / 2.f;
    float startY = 180.f;

    for (size_t i = 0; i < metrics.size(); i++) {
        int col = i % cols;
        int row = i / cols;
        auto card = createMetricCard(metrics[i].label, metrics[i].value, metrics[i].color, i * 0.03f);
        card->setPosition(startX + col * (cardW + gapX) + cardW / 2.f, startY - row * (cardH + gapY) + cardH / 2.f);
        this->addChild(card);
    }
}

bool StatsTab::init(float width, float height) {
    if (!CCNode::init()) return false;
    this->setContentSize({width, height});
    return true;
}

StatsTab* StatsTab::create(float width, float height) {
    auto ret = new StatsTab();
    if (ret->init(width, height)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}