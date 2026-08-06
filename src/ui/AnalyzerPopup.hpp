#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/utils/async.hpp>
#include "../Replay.hpp"
#include "tabs/Stats/StatsTab.hpp"
#include "tabs/Analysis/AnalysisTab.hpp"

using namespace geode::prelude;

class AnalyzerPopup : public Popup {
protected:
    bool init(float width, float height);
    arc::TaskHandle<void> m_pickHandle;
    StatsTab* m_statsTab = nullptr;
    AnalysisTab* m_analysisTab = nullptr;
    CCMenuItemSpriteExtra* m_statsTabBtn = nullptr;
    CCMenuItemSpriteExtra* m_analysisTabBtn = nullptr;
    CCNode* m_loadingContainer = nullptr;
    CCLabelBMFont* m_errorLabel = nullptr;
    Replay m_currentReplay;
    bool m_hasReplay = false;
    void showLoading();
    void hideLoading();
    void showError(std::string const& message);
    void switchTab(bool showAnalysis);
    void onSwitchTab(CCObject* sender);

public:
    static AnalyzerPopup* create();
    void onOpenFile(CCObject* sender);
};