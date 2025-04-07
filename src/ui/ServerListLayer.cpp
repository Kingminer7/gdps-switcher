#include "ServerListLayer.hpp"
#include "GUI/CCControlExtension/CCScale9Sprite.h"
#include "Geode/binding/MenuLayer.hpp"
#include "Geode/cocos/CCDirector.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCTransition.h"
#include "Geode/cocos/misc_nodes/CCClippingNode.h"
#include "Geode/loader/Loader.hpp"
#include "Geode/loader/Mod.hpp"
#include "Geode/ui/Layout.hpp"
#include "Geode/ui/ScrollLayer.hpp"
#include "Geode/ui/Scrollbar.hpp"
#include "Types.hpp"
#include "km7dev.server_api/include/ServerAPIEvents.hpp"
#include "ui/ServerNode.hpp"
#include <algorithm>

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

    auto scrollBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    scrollBg->setColor({0, 0, 0});
    scrollBg->setOpacity(90);
    scrollBg->setContentSize({380, 240});
    scrollBg->ignoreAnchorPointForPosition(false);
    scrollBg->setID("server-scroll-bg");
    this->addChildAtPosition(scrollBg, geode::Anchor::Center, {0, 0}, false);
    
    m_scroll = geode::ScrollLayer::create({360, 235});
    m_scroll->setID("server-scroll");
    m_scroll->ignoreAnchorPointForPosition(false);
    m_scrollbar = geode::Scrollbar::create(m_scroll);
    m_scrollbar->setID("scrollbar");
    m_scrollbar->setScaleY(.95f);

    auto clip = cocos2d::CCClippingNode::create();
    clip->setID("server-list");
    clip->setZOrder(1);
    clip->setContentSize({383, 235});
    clip->setAnchorPoint({0.5, 0.5});
    clip->setAlphaThreshold(0.05f);
    clip->addChildAtPosition(m_scroll, geode::Anchor::Center, {-5.f, 0.f});
    clip->addChildAtPosition(m_scrollbar, geode::Anchor::Right, {-6.f, 0.f});
    addChildAtPosition(clip, geode::Anchor::Center, { -.5f, 0.f}, false);

    auto stencil = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    stencil->setContentSize({380, 235});
    stencil->setPosition({190.f, 117.5f});
    clip->setStencil(stencil);

    auto servers = geode::Mod::get()->getSavedValue<std::vector<GDPSTypes::Server>>("saved-servers");
    servers.insert(servers.begin(), {"Built-in Servers", ServerAPIEvents::getBaseUrl()});
    m_scroll->m_contentLayer->setContentSize({360, std::max(servers.size() * 80.f - 5.f, 235.f)});
    m_scroll->scrollToTop();
    float y = -5.f;
    for (auto server : servers) {
        auto node = ServerNode::create(server, {360, 75});
        y += 80.f;
        m_scroll->m_contentLayer->addChildAtPosition(node, geode::Anchor::Top, {0, 37.5f - y}, false);
    }

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