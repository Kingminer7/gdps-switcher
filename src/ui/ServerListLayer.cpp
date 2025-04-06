#include "ServerListLayer.hpp"
#include "Geode/binding/MenuLayer.hpp"
#include "Geode/cocos/CCDirector.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCTransition.h"
#include "Geode/cocos/misc_nodes/CCClippingNode.h"
#include "Geode/ui/Layout.hpp"

  bool ServerListLayer::init() {
    if (!CCLayer::init()) return false;

    auto winSize = cocos2d::CCDirector::get()->getWinSize();
    this->setID("ServerListLayer"_spr);
    this->setKeyboardEnabled(true);
    this->setKeypadEnabled(true);

    auto bg = geode::createLayerBG();
    bg->setID("background");
    this->addChild(bg);

    auto m_backMenu = cocos2d::CCMenu::create();
    m_backMenu->setID("back-menu");
    m_backMenu->setContentHeight(100.f);
    m_backMenu->setAnchorPoint({ .5f, 1.f });
    
    auto backSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    auto backBtn = CCMenuItemSpriteExtra::create(
        backSpr, this, menu_selector(ServerListLayer::onBack)
    );
    backBtn->setID("back-button");
    m_backMenu->addChild(backBtn);

    m_backMenu->setLayout(
        geode::ColumnLayout::create()
            ->setAxisAlignment(geode::AxisAlignment::End)
            ->setAxisReverse(true)
    );
    this->addChildAtPosition(m_backMenu, geode::Anchor::TopLeft, ccp(25.f, -25.f / 4.f), false);

    return true;
}

void ServerListLayer::keyBackClicked() {
    this->onBack(nullptr);
}

void ServerListLayer::onBack(cocos2d::CCObject *sender) {
    cocos2d::CCDirector::get()->replaceScene(cocos2d::CCTransitionFade::create(.5f, MenuLayer::scene(false)));
}

ServerListLayer *ServerListLayer::create() {
    auto ret = new ServerListLayer;
    if (ret && ret->init()) {
      ret->autorelease();
      return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

cocos2d::CCScene *ServerListLayer::scene() {
    auto scene = cocos2d::CCScene::create();
    scene->addChild(ServerListLayer::create());
    return scene;
}