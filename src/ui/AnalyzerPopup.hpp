#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/utils/async.hpp>
#include "../Replay.hpp"

using namespace geode::prelude;

class AnalyzerPopup : public Popup {
protected:
    bool init(float width, float height);
    arc::TaskHandle<void> m_pickHandle;
    CCNode* m_statsContainer = nullptr;
    CCNode* m_loadingContainer = nullptr;
    CCLabelBMFont* m_errorLabel = nullptr;
    CCNode* createMetricCard(std::string const& label, std::string const& value, ccColor3B color, float delay);
    void populateStats(Replay const& replay);
    void showLoading();
    void hideLoading();
    void showError(std::string const& message);

public:
    static AnalyzerPopup* create();
    void onOpenFile(CCObject* sender);
};