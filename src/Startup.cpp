#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include "ui/AnalyzerPopup.hpp"

using namespace geode::prelude;

class $modify(MyCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto menu = CCMenu::create();
        auto icon = CCSprite::createWithSpriteFrameName("GJ_statsBtn_001.png");
        icon->setScale(0.8f);
        auto btn = CCMenuItemSpriteExtra::create(
            icon,
            this,
            menu_selector(MyCreatorLayer::onMacroAnalyzer)
        );
        menu->addChild(btn);
        menu->setPosition({winSize.width - 22.f, winSize.height - 100.f});
        menu->setID("macro-analyzer-menu"_spr);
        this->addChild(menu);

        return true;
    }

    void onMacroAnalyzer(CCObject* sender) {
        AnalyzerPopup::create()->show();
    }
};