#include "AnalyzerPopup.hpp"
#include "../parsers/Gdr2/Gdr2Parser.hpp"
#include "../parsers/Json/JsonParser.hpp"
#include "../parsers/Gdr/GdrParser.hpp"
#include <fmt/format.h>

void AnalyzerPopup::showLoading() {
    m_errorLabel->setVisible(false);
    m_statsTab->setVisible(false);
    m_analysisTab->setVisible(false);
    m_loadingContainer->setVisible(true);
}

void AnalyzerPopup::hideLoading() {
    m_loadingContainer->setVisible(false);
    switchTab(false);
}

void AnalyzerPopup::showError(std::string const& message) {
    m_statsTab->setVisible(false);
    m_analysisTab->setVisible(false);
    m_loadingContainer->setVisible(false);
    m_errorLabel->setString(message.c_str());
    m_errorLabel->setVisible(true);
}

void AnalyzerPopup::switchTab(bool showAnalysis) {
    m_statsTab->setVisible(!showAnalysis);
    m_analysisTab->setVisible(showAnalysis);
    m_statsTabBtn->setOpacity(showAnalysis ? 160 : 255);
    m_analysisTabBtn->setOpacity(showAnalysis ? 255 : 160);
    if (m_hasReplay) {
        if (showAnalysis) {
            m_analysisTab->populate(m_currentReplay);
        } else {
            m_statsTab->populate(m_currentReplay);
        }
    }
}

void AnalyzerPopup::onSwitchTab(CCObject* sender) {
    switchTab(sender == m_analysisTabBtn);
}

bool AnalyzerPopup::init(float width, float height) {
    if (!Popup::init(width, height)) return false;

    this->setTitle("Macro Analyzer");

    auto openFileBtnNode = CCNodeRGBA::create();
    openFileBtnNode->setContentSize({26.f, 26.f});

    auto openFilePanel = NineSlice::create("GJ_square01.png");
    openFilePanel->setContentSize(openFileBtnNode->getContentSize());
    openFilePanel->setPosition(openFileBtnNode->getContentSize() / 2);
    openFilePanel->setScaleMultiplier(0.5f);
    openFileBtnNode->addChild(openFilePanel);

    auto openFileIcon = CCSprite::createWithSpriteFrameName("icon_file.png"_spr);
    openFileIcon->setScale(0.5f);
    openFileIcon->setPosition(openFileBtnNode->getContentSize() / 2.f);
    openFileBtnNode->addChild(openFileIcon);

    auto btn = CCMenuItemSpriteExtra::create(
        openFileBtnNode,
        this,
        menu_selector(AnalyzerPopup::onOpenFile)
    );
    m_buttonMenu->addChild(btn);

    float sidebarWidth = 100.f;
    float contentWidth = m_mainLayer->getContentSize().width - sidebarWidth;
    float contentHeight = m_mainLayer->getContentSize().height;

    btn->setPosition({m_mainLayer->getContentSize().width - 20.f, contentHeight - 20.f});

    m_statsTab = StatsTab::create(contentWidth, contentHeight);
    m_statsTab->setPosition(sidebarWidth, 0.f);
    m_mainLayer->addChild(m_statsTab);

    m_analysisTab = AnalysisTab::create(contentWidth, contentHeight);
    m_analysisTab->setPosition(sidebarWidth, 0.f);
    m_analysisTab->setVisible(false);
    m_mainLayer->addChild(m_analysisTab);

    auto sidebarBg = NineSlice::create("GJ_square01.png");
    sidebarBg->setContentSize({sidebarWidth, contentHeight});
    sidebarBg->setAnchorPoint({0.f, 0.f});
    sidebarBg->setPosition(0.f, 0.f);
    sidebarBg->setColor({30, 30, 35});
    sidebarBg->setOpacity(180);
    m_mainLayer->addChild(sidebarBg);

    auto createSidebarRow = [](std::string const& icon, std::string const& text) -> CCNode* {
        auto row = CCNodeRGBA::create();
        row->setCascadeOpacityEnabled(true);
        row->setContentSize({90.f, 34.f});

        auto panel = NineSlice::create("GJ_square01.png");
        panel->setContentSize(row->getContentSize());
        panel->setPosition(row->getContentSize() / 2);
        row->addChild(panel);

        auto iconSprite = CCSprite::createWithSpriteFrameName(icon.c_str());
        iconSprite->setScale(0.4f);
        iconSprite->setPosition({16.f, row->getContentSize().height / 2.f});
        row->addChild(iconSprite);

        auto label = CCLabelBMFont::create(text.c_str(), "chatFont.fnt");
        label->setScale(0.35f);
        label->setAnchorPoint({0.f, 0.5f});
        label->setPosition(30.f, row->getContentSize().height / 2.f);
        row->addChild(label);

        return row;
    };

    auto sidebarMenu = CCMenu::create();
    sidebarMenu->setPosition(0.f, 0.f);

    m_statsTabBtn = CCMenuItemSpriteExtra::create(
        createSidebarRow("tab_dashboard.png"_spr, "Stats"),
        this, menu_selector(AnalyzerPopup::onSwitchTab)
    );
    m_statsTabBtn->setPosition(50.f, contentHeight - 40.f);
    sidebarMenu->addChild(m_statsTabBtn);

    m_analysisTabBtn = CCMenuItemSpriteExtra::create(
        createSidebarRow("tab_analysis.png"_spr, "Analysis"),
        this, menu_selector(AnalyzerPopup::onSwitchTab)
    );
    m_analysisTabBtn->setPosition(50.f, contentHeight - 82.f);
    sidebarMenu->addChild(m_analysisTabBtn);

    m_mainLayer->addChild(sidebarMenu);

    switchTab(false);

    m_loadingContainer = CCNode::create();
    m_loadingContainer->setPosition(m_mainLayer->getContentSize().width / 2.f, m_mainLayer->getContentSize().height / 2.f);
    m_loadingContainer->setVisible(false);
    m_mainLayer->addChild(m_loadingContainer);

    auto spinner = CCSprite::create("loadingCircle.png");
    spinner->setScale(0.8f);
    spinner->setPosition({0.f, 15.f});
    spinner->runAction(CCRepeatForever::create(CCRotateBy::create(1.f, 360.f)));
    m_loadingContainer->addChild(spinner);

    auto loadingLabel = CCLabelBMFont::create("Parsing File...", "goldFont.fnt");
    loadingLabel->setScale(0.6f);
    loadingLabel->setPosition(0.f, -25.f);
    m_loadingContainer->addChild(loadingLabel);

    m_errorLabel = CCLabelBMFont::create("", "chatFont.fnt");
    m_errorLabel->setPosition(m_mainLayer->getContentSize().width / 2.f, m_mainLayer->getContentSize().height / 2.f);
    m_errorLabel->setColor({255, 90, 90});
    m_errorLabel->setVisible(false);
    m_mainLayer->addChild(m_errorLabel);

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
                log::error("File Pick Failed: {}", result.unwrapErr());
                return;
            }
            auto path = result.unwrap();
            if (!path.has_value()) {
                log::info("File Pick Cancelled");
                return;
            }
            log::info("Picked File: {}", path.value().string());

            auto filePath = path.value();
            auto ext = filePath.extension().string();
            Result<Replay> parseResult = [&]() {
                if (ext == ".json") return JsonParser::parse(filePath);
                if (ext == ".gdr") return GdrParser::parse(filePath);
                return Gdr2Parser::parse(filePath);
            }();
            if (!parseResult) {
                log::error("Parse Failed: {}", parseResult.unwrapErr());
                self->showError("Failed To Parse File");
                return;
            }
            auto replay = parseResult.unwrap();
            log::info("Parsed Replay: Author={}, Level={}, Frames={}, Duration={}ms",
                replay.author, replay.levelName, replay.frames.size(), replay.duration);

            self->m_currentReplay = replay;
            self->m_hasReplay = true;
            self->m_statsTab->populate(replay);
            self->m_analysisTab->populate(replay);
            self->switchTab(false);
        }
    );
}