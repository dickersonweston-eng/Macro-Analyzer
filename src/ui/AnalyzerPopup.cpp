#include "AnalyzerPopup.hpp"
#include "../parsers/Gdr2Parser.hpp"
#include <fmt/format.h>

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
    m_statsLabel = CCLabelBMFont::create("Pick A File To See Stats", "chatFont.fnt");
    m_statsLabel->setScale(0.65f);
    m_statsLabel->setPosition(m_mainLayer->getContentSize().width / 2, 185.f);
    m_statsLabel->setAnchorPoint({0.5f, 1.0f});
    m_mainLayer->addChild(m_statsLabel);

    return true;
}

AnalyzerPopup* AnalyzerPopup::create() {
    auto ret = new AnalyzerPopup();
    if (ret->init(320.f, 240.f)) {
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
            std::string statsText = fmt::format(
                "Author: {}\nLevel: {}\nDuration: {:.1f}s\nFrames: {}\nCoins: {}\nPlatformer: {}",
                replay.author,
                replay.levelName,
                replay.duration / 1000.0,
                replay.frames.size(),
                replay.coins,
                replay.platformer ? "Yes" : "No"
            );
            self->m_statsLabel->setString(statsText.c_str());
        }
    );
}