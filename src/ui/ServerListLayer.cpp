#include "ServerListLayer.hpp"
#include "ModifyServerPopup.hpp"
#include "ServerNode.hpp"
#include "Types.hpp"
#include "utils/GDPSMain.hpp"

#include <km7dev.server_api/include/ServerAPIEvents.hpp>

GDPSTypes::Server ServerListLayer::m_selectedServer = {"", ""};

bool ServerListLayer::init() {
    if (!CCLayer::init()) return false;

    if (m_selectedServer.empty()) m_selectedServer = GDPSMain::get()->m_currentServer;

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

    auto m_bottomMenu = cocos2d::CCMenu::create();
    m_bottomMenu->setID("edit-menu");
    m_bottomMenu->setContentHeight(125.f);
    m_bottomMenu->setAnchorPoint({ .5f, 0.f });

    auto addSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_newBtn_001.png");
    addSpr->setScale(.7f);
    auto addBtn = CCMenuItemSpriteExtra::create(
        addSpr, this, menu_selector(ServerListLayer::onAdd)
    );
    addBtn->setID("add-button");
    m_bottomMenu->addChild(addBtn);

    auto editSpr = CCSprite::createWithSpriteFrameName("edit.png"_spr);
    auto editCircle = CircleButtonSprite::create(editSpr, CircleBaseColor::Green, CircleBaseSize::Big);
    editSpr->setScale(1.4f);
    editCircle->setScale(.7f);
    auto editBtn = CCMenuItemSpriteExtra::create(
        editCircle, this, menu_selector(ServerListLayer::onEdit)
    );
    editBtn->setID("edit-button");
    m_bottomMenu->addChild(editBtn);

    m_bottomMenu->setLayout(
        geode::ColumnLayout::create()
            ->setAxisAlignment(geode::AxisAlignment::Start)
            ->setAxisReverse(false)
            ->setGap(3.f)
    );
    this->addChildAtPosition(m_bottomMenu, geode::Anchor::BottomRight, ccp(-25.f, 25.f / 4.f), false);

    auto scrollBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    scrollBg->setColor({0, 0, 0});
    scrollBg->setOpacity(90);
    scrollBg->setContentSize({380, 240});
    scrollBg->ignoreAnchorPointForPosition(false);
    scrollBg->setID("server-scroll-bg");
    this->addChildAtPosition(scrollBg, geode::Anchor::Center, {0, 0}, false);
    
    m_scroll = geode::ScrollLayer::create({363, 235});
    m_scroll->setID("server-scroll");
    m_scroll->ignoreAnchorPointForPosition(false);
    m_scrollbar = geode::Scrollbar::create(m_scroll);
    m_scrollbar->setID("scrollbar");
    m_scrollbar->setScaleY(.95f);

    auto clip = cocos2d::CCClippingNode::create();
    clip->setID("server-list");
    clip->setZOrder(1);
    clip->setContentSize({377, 235});
    clip->setAnchorPoint({0.5, 0.5});
    clip->setAlphaThreshold(0.05f);
    clip->addChildAtPosition(m_scroll, geode::Anchor::Center, {-5.f, 0.f});
    clip->addChildAtPosition(m_scrollbar, geode::Anchor::Right, {-6.f, 0.f});
    addChildAtPosition(clip, geode::Anchor::Center, { 1.f, 0.f}, false);

    auto stencil = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    stencil->setContentSize({377, 235});
    stencil->setPosition({190.f, 117.5f});
    clip->setStencil(stencil);

    updateList();

    return true;
}

void ServerListLayer::updateList() {
    m_servers = GDPSMain::get()->m_servers;
    m_servers.insert(m_servers.begin(), {"Built-in Servers", ServerAPIEvents::getBaseUrl()});
    m_scroll->m_contentLayer->removeAllChildren();
    m_scroll->m_contentLayer->setContentSize({363, std::max(m_servers.size() * 80.f - 5.f, 235.f)});
    m_scroll->scrollToTop();
    float y = -5.f;
    for (auto server : m_servers) {
        auto node = ServerNode::create(server, {363, 75}, this);
        node->setEditing(m_isEditing);
        y += 80.f;
        m_scroll->m_contentLayer->addChildAtPosition(node, geode::Anchor::Top, {0, 37.5f - y}, false);
    }
}

void ServerListLayer::keyBackClicked() {
    this->onBack(nullptr);
}

void ServerListLayer::onBack(cocos2d::CCObject *sender) {
    if (m_selectedServer != GDPSMain::get()->m_currentServer) {
        geode::createQuickPopup("GDPS Switcher", "Switching servers requires a restart.", "Restart later", "Restart now", [this](auto, bool now) {
            if (now) {
                geode::utils::game::restart();
            } else {
                cocos2d::CCDirector::get()->replaceScene(cocos2d::CCTransitionFade::create(.5f, MenuLayer::scene(false)));
            }
        });
    } else {
        cocos2d::CCDirector::get()->replaceScene(cocos2d::CCTransitionFade::create(.5f, MenuLayer::scene(false)));
    }
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

void ServerListLayer::onSelect(GDPSTypes::Server server) {
    m_selectedServer = server;
    for (auto node : CCArrayExt<ServerNode>(m_scroll->m_contentLayer->getChildren())) {
        if (!node) return;
        node->updateSelected(server);
        Mod::get()->setSavedValue("server", server.url);
    }
}

void ServerListLayer::onAdd(CCObject *sender) {
    ModifyServerPopup::create({"", ""}, this)->show();
}

void ServerListLayer::onEdit(CCObject *sender) {
    m_isEditing = !m_isEditing;
    for (auto node : CCArrayExt<ServerNode>(m_scroll->m_contentLayer->getChildren())) {
        if (!node) return;
        node->setEditing(m_isEditing);
    }
}

void ServerListLayer::keyDown(enumKeyCodes code) {
    CCLayer::keyDown(code);
    static const std::vector<enumKeyCodes> sequence = {
        enumKeyCodes::KEY_Up, enumKeyCodes::KEY_Up, 
        enumKeyCodes::KEY_Down, enumKeyCodes::KEY_Down, 
        enumKeyCodes::KEY_Left, enumKeyCodes::KEY_Right, 
        enumKeyCodes::KEY_Left, enumKeyCodes::KEY_Right, 
        enumKeyCodes::KEY_B, enumKeyCodes::KEY_A, 
        enumKeyCodes::KEY_Space, enumKeyCodes::KEY_Enter
    };

    if (m_eePos >= sequence.size()) return;

    if (code == sequence[m_eePos]) {
        m_eePos++;
    } else {
        m_eePos = 0;
        return;
    }

    if (m_eePos == sequence.size()) {
        // TODO: impl this
    }
}