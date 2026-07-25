#include "AnalyzerPopup.hpp"
#include "../parsers/Gdr2Parser.hpp"
#include <fmt/format.h>

CCNode* AnalyzerPopup::createMetricCard(std::string const& label, std::string const& value, ccColor3B color) {
    auto card = CCNode::create();
    card->setContentSize({108.f, 46.f});

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
    valueText->setScale(0.5f);
    valueText->setColor(color);
    valueText->setAnchorPoint({0.f, 1.f});
    valueText->setPosition(10.f, card->getContentSize().height - 24.f);
    card->addChild(valueText);

    return card;
}

void AnalyzerPopup::populateStats(Replay const& replay) {
    m_statsContainer->removeAllChildren();

    struct Metric { std::string label; std::string value; ccColor3B color; };
    std::vector<Metric> metrics = {
        { "Author", replay.author, {140, 255, 140} },
        { "Bot", replay.botName + " v" + replay.botVersion, {140, 255, 140} },
        { "Duration", fmt::format("{:.1f}s", replay.duration / 1000.0), {140, 255, 140} },
        { "Framerate", fmt::format("{} fps", (int)replay.frameRate), {120, 230, 255} },
        { "Inputs", std::to_string(replay.frames.size()), {140, 255, 140} },
        { "Coins", std::to_string(replay.coins), {140, 255, 140} },
        { "Deaths", std::to_string(replay.deaths.size()), {140, 255, 140} },
        { "Mode", replay.platformer ? "Platformer" : "Classic", {255, 140, 255} },
        { "LDM", replay.ldm ? "Yes" : "No", {255, 140, 255} },
        { "Level ID", std::to_string(replay.levelId), {140, 255, 140} },
        { "Level", replay.levelName, {140, 255, 140} },
        { "Format", "GDR2", {140, 255, 140} },
    };

    int cols = 4;
    float cardW = 108.f, cardH = 46.f, gapX = 12.f, gapY = 8.f;
    float gridW = cols * cardW + (cols - 1) * gapX;
    float startX = (m_mainLayer->getContentSize().width - gridW) / 2.f;
    float startY = 180.f;

    for (size_t i = 0; i < metrics.size(); i++) {
        int col = i % cols;
        int row = i / cols;
        auto card = createMetricCard(metrics[i].label, metrics[i].value, metrics[i].color);
        card->setPosition(startX + col * (cardW + gapX), startY - row * (cardH + gapY));
        m_statsContainer->addChild(card);
    }
}

bool AnalyzerPopup::init(float width, float height) {
    if (!Popup::init(width, height)) return false;

    this->setTitle("Macro Analyzer");

    auto openFileBtn = ButtonSprite::create("Open File");
    auto btn = CCMenuItemSpriteExtra::create(
        openFileBtn,
        this,
        menu_selector(AnalyzerPopup::onOpenFile)
    );
    btn->setPosition(m_mainLayer->getContentSize().width / 2, 30.f);
    m_buttonMenu->addChild(btn);
    m_statsContainer = CCNode::create();
    m_statsContainer->setPosition(0.f, 0.f);
    m_mainLayer->addChild(m_statsContainer);

    return true;
}

AnalyzerPopup* AnalyzerPopup::create() {
    auto ret = new AnalyzerPopup();
    if (ret->init(520.f, 285.f)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void AnalyzerPopup::onOpenFile(CCObject* sender) {
    file::FilePickOptions options = {
        .filters = {
            { .description = "GD Macro Files", .files = { "*.gdr2", "*.gdr", "*.json" } }
        }
    };

    auto future = file::pick(file::PickMode::OpenFile, options);
    m_pickHandle = geode::async::spawn(
        std::move(future),
        [self = Ref<AnalyzerPopup>(this)](file::PickResult result) {
            if (!result) {
                log::error("File pick failed: {}", result.unwrapErr());
                return;
            }
            auto path = result.unwrap();
            if (!path.has_value()) {
                log::info("File pick cancelled");
                return;
            }
            log::info("Picked file: {}", path.value().string());

            auto parseResult = Gdr2Parser::parse(path.value());
            if (!parseResult) {
                log::error("Parse failed: {}", parseResult.unwrapErr());
                return;
            }
            auto replay = parseResult.unwrap();
            log::info("Parsed replay: author={}, level={}, frames={}, duration={}ms",
                replay.author, replay.levelName, replay.frames.size(), replay.duration);
            self->populateStats(replay);
        }
    );
}