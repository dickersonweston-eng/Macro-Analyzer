#include "AnalysisTab.hpp"
#include <fmt/format.h>
#include <algorithm>

CCNode* AnalysisTab::createInputTypeChart(Replay const& replay, float width, float height) {
    auto container = CCNode::create();
    container->setContentSize({width, height});
    container->setAnchorPoint({0.f, 0.f});

    auto title = CCLabelBMFont::create("INPUT TYPE BREAKDOWN", "chatFont.fnt");
    title->setScale(0.4f);
    title->setAnchorPoint({0.f, 1.f});
    title->setPosition(0.f, height);
    title->setOpacity(0);
    title->runAction(CCFadeTo::create(0.3f, 255));
    container->addChild(title);

    int jump = 0, left = 0, right = 0;
    for (auto& frame : replay.frames) {
        if (!frame.down) continue;
        if (frame.button == 1) jump++;
        else if (frame.button == 2) left++;
        else if (frame.button == 3) right ++;
    }
    int maxCount = std::max({jump, left, right, 1});

    struct BarData { std::string label; int count; ccColor3B color; };
    std::vector<BarData> bars = {
        { "Jump", jump, {140, 255, 140} },
        { "Left", left, {120, 230, 255} },
        { "Right", right, {255, 140, 255} },
    };

    float barMaxWidth = width - 90.f;
    float rowHeight = 20.f;
    float startY = height - 20.f;

    for (size_t i = 0; i < bars.size(); i++) {
        float y = startY - i * rowHeight;
        float delay = i * 0.08f;

        auto label = CCLabelBMFont::create(bars[i].label.c_str(), "chatFont.fnt");
        label->setScale(0.35f);
        label->setAnchorPoint({0.f, 0.5f});
        label->setPosition(0.f, y);
        label->setOpacity(0);
        label->runAction(CCSequence::create(CCDelayTime::create(delay), CCFadeTo::create(0.3f, 255), nullptr));
        container->addChild(label);

        float barWidth = std::max(2.f, (bars[i].count / (float)maxCount) * barMaxWidth);
        auto bar = CCLayerColor::create(ccc4(bars[i].color.r, bars[i].color.g, bars[i].color.b, 255), barWidth, 12.f);
        bar->setAnchorPoint({0.f, 0.5f});
        bar->setPosition(50.f, y);
        bar->setOpacity(0);
        bar->runAction(CCSequence::create(CCDelayTime::create(delay), CCFadeTo::create(0.3f, 255), nullptr));
        container->addChild(bar);

        auto countLabel = CCLabelBMFont::create(std::to_string(bars[i].count).c_str(), "chatFont.fnt");
        countLabel->setScale(0.3f);
        countLabel->setAnchorPoint({0.f, 0.5f});
        countLabel->setPosition(50.f + barMaxWidth + 6.f, y);
        countLabel->setOpacity(0);
        countLabel->runAction(CCSequence::create(CCDelayTime::create(delay), CCFadeTo::create(0.3f, 255), nullptr));
        container->addChild(countLabel);
    }
    return container;
}

CCNode* AnalysisTab::createInputsOverTimeChart(Replay const& replay, float width, float height) {
    auto container = CCNode::create();
    container->setContentSize({width, height});
    container->setAnchorPoint({0.f, 0.f});

    auto title = CCLabelBMFont::create("INPUTS OVER TIME", "chatFont.fnt");
    title->setScale(0.4f);
    title->setAnchorPoint({0.f, 1.f});
    title->setPosition(0.f, height);
    title->setOpacity(0);
    title->runAction(CCFadeTo::create(0.3f, 255));
    container->addChild(title);

    int bucketCount = 24;
    double durationSec = std::max(replay.duration / 1000.0, 1.0);
    double bucketSize = durationSec / bucketCount;
    std::vector<int> buckets(bucketCount, 0);

    for (auto& frame : replay.frames) {
        if (!frame.down) continue;
        int idx = std::min(bucketCount - 1, std::max(0, (int)((frame.timeStampMs / 1000.0) / bucketSize)));
        buckets[idx]++;
    }

    int maxCount = 1;
    for (int c : buckets) maxCount = std::max(maxCount, c);

    float axisHeight = 14.f;
    float chartHeight = height - 24.f - axisHeight;
    float gap = 2.f;
    float barWidth = (width - (bucketCount - 1) * gap) / bucketCount;

    ccColor3B lowColor = {100, 200, 255};
    ccColor3B highColor = {200, 120, 255};

    for (int i = 0; i < bucketCount; i++) {
        float t = buckets[i] / (float)maxCount;
        float barHeight = std::max(2.f, t * chartHeight);
        ccColor3B color = {
            (GLubyte) (lowColor.r + (highColor.r - lowColor.r) * t),
            (GLubyte) (lowColor.g + (highColor.g - lowColor.g) * t),
            (GLubyte) (lowColor.b + (highColor.b - lowColor.b) * t)
        };
        auto bar = CCLayerColor::create(ccc4(color.r, color.g, color.b, 255), barWidth, barHeight);
        bar->setAnchorPoint({0.f, 0.f});
        bar->setPosition(i * (barWidth + gap), axisHeight);
        bar->setOpacity(0);
        bar->runAction(CCSequence::create(
            CCDelayTime::create(i * 0.02f),
            CCFadeTo::create(0.25f, 255),
            nullptr
        ));
        container->addChild(bar);
    }

    int labelCount = 4;
    for (int i = 0; i < labelCount; i++) {
        float frac = i / (float)(labelCount - 1);
        double seconds = frac * durationSec;
        auto label = CCLabelBMFont::create(fmt::format("{:.0f}s", seconds).c_str(), "chatFont.fnt");
        label->setScale(0.3f);
        label->setAnchorPoint({frac == 0.f ? 0.f : (frac == 1.f ? 1.f : 0.5f), 0.f});
        label->setPosition(frac * width, 0.f);
        container->addChild(label);
    }
    return container;
}

void AnalysisTab::populate(Replay const& replay) {
    this->removeAllChildren();

    float width = this->getContentSize().width;
    float height = this->getContentSize().height;

    float breakdownHeight = 90.f;
    auto breakdownChart = createInputTypeChart(replay, width - 20.f, breakdownHeight);
    breakdownChart->setPosition(10.f, height - breakdownHeight - 35.f);
    this->addChild(breakdownChart);

    float overTimeHeight = height - breakdownHeight - 30.f;
    auto overTimeChart = createInputsOverTimeChart(replay, width - 20.f, overTimeHeight);
    overTimeChart->setPosition(10.f, 10.f);
    this->addChild(overTimeChart);
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