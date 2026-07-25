#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/file.hpp>
#include <Geode/utils/async.hpp>

using namespace geode::prelude;

class AnalyzerPopup : public Popup {
protected:
    bool init(float width, float height);
    arc::TaskHandle<void> m_pickHandle;
    CCLabelBMFont* m_statsLabel = nullptr;

public:
    static AnalyzerPopup* create();
    void onOpenFile(CCObject* sender);
};