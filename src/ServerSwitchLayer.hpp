#pragma once

#include <Geode/Geode.hpp>

class PackNode;

using namespace geode::prelude;

class ServerSwitchLayer : public CCLayer {
protected:
    bool init() override;
    void keyBackClicked() override;
public:
    static ServerSwitchLayer* create();
    static cocos2d::CCLayer* scene();
    CCTextInputNode* input;
    void onGoBack(CCObject*);
    void onApply(CCObject*);
};
