#include "ServerListLayer.hpp"
#include "ModifyServerPopup.hpp"
#include "ServerNode.hpp"
#include "Types.hpp"
#include "utils/GDPSMain.hpp"

#include <km7dev.server_api/include/ServerAPIEvents.hpp>

int ServerListLayer::m_selectedServer = -1;

bool ServerListLayer::init() {
    if (!CCLayer::init()) return false;

    registerWithTouchDispatcher();

    if (m_selectedServer == -1) m_selectedServer = GDPSMain::get()->m_currentServer;

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

    m_bottomMenu = cocos2d::CCMenu::create();
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

    if (Mod::get()->getSavedValue("secret-settings", false)) {
        m_eePos = 12;

        auto spr = CCSprite::createWithSpriteFrameName("geode.loader/settings.png");
        auto circle = CircleButtonSprite::create(spr, CircleBaseColor::Green, CircleBaseSize::Big);
        circle->setScale(.7f);
        auto btn = CCMenuItemSpriteExtra::create(
            circle, this, menu_selector(ServerListLayer::onSettings)
        );
        btn->setID("settings-button");
        m_bottomMenu->addChild(btn);
        m_bottomMenu->updateLayout();
    }

    return true;
}

void ServerListLayer::updateList() {
    m_servers = GDPSMain::get()->m_servers;
    m_scroll->m_contentLayer->removeAllChildren();
    m_scroll->m_contentLayer->setContentSize({363, std::max(m_servers.size() * 80.f - 5.f, 235.f)});
    m_scroll->scrollToTop();
    float y = -5.f;
    bool odd = false;
    for (auto &[id, server] : m_servers) {
        odd = !odd;
        auto node = ServerNode::create(server, {363, 75}, this, odd);
        if (server.id == -2) {
            node->m_locked = true;
        }
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
    m_selectedServer = server.id;
    for (auto node : CCArrayExt<ServerNode>(m_scroll->m_contentLayer->getChildren())) {
        if (!node) return;
        node->updateSelected(server);
        Mod::get()->setSavedValue("current", server.id);
    }
}

void ServerListLayer::onAdd(CCObject *sender) {
    int id = 0;
    for (auto &[serverId, server] : m_servers) {
      log::info("Server ID: {}, ID: {}", serverId, id);
        if (serverId < 0) continue;
        if (serverId == id) id++;
        else break;
    }
    ModifyServerPopup::create({id, "", "", ""}, this)->show();
}

void ServerListLayer::onEdit(CCObject *sender) {
    m_isEditing = !m_isEditing;
    for (auto node : CCArrayExt<ServerNode>(m_scroll->m_contentLayer->getChildren())) {
        if (!node) return;
        node->setEditing(m_isEditing);
    }
}


// Konami code easter egg

void ServerListLayer::keyDown(enumKeyCodes code) {
  static const std::vector<std::vector<enumKeyCodes>> sequence = {
        {enumKeyCodes::KEY_Up, enumKeyCodes::CONTROLLER_Up,
        enumKeyCodes::CONTROLLER_LTHUMBSTICK_UP, CONTROLLER_RTHUMBSTICK_UP},
        {enumKeyCodes::KEY_Up, enumKeyCodes::CONTROLLER_Up,
        enumKeyCodes::CONTROLLER_LTHUMBSTICK_UP, CONTROLLER_RTHUMBSTICK_UP},
        {enumKeyCodes::KEY_Down, enumKeyCodes::CONTROLLER_Down,
        enumKeyCodes::CONTROLLER_LTHUMBSTICK_DOWN, CONTROLLER_RTHUMBSTICK_DOWN},
        {enumKeyCodes::KEY_Down, enumKeyCodes::CONTROLLER_Down,
        enumKeyCodes::CONTROLLER_LTHUMBSTICK_DOWN, CONTROLLER_RTHUMBSTICK_DOWN},
        {enumKeyCodes::KEY_Left, enumKeyCodes::CONTROLLER_Left,
        enumKeyCodes::CONTROLLER_LTHUMBSTICK_LEFT, CONTROLLER_RTHUMBSTICK_LEFT},
        {enumKeyCodes::KEY_Right, enumKeyCodes::CONTROLLER_Right,
        enumKeyCodes::CONTROLLER_LTHUMBSTICK_RIGHT,
        CONTROLLER_RTHUMBSTICK_RIGHT},
        {enumKeyCodes::KEY_Left, enumKeyCodes::CONTROLLER_Left,
        enumKeyCodes::CONTROLLER_LTHUMBSTICK_LEFT, CONTROLLER_RTHUMBSTICK_LEFT},
        {enumKeyCodes::KEY_Right, enumKeyCodes::CONTROLLER_Right,
        enumKeyCodes::CONTROLLER_LTHUMBSTICK_RIGHT,
        CONTROLLER_RTHUMBSTICK_RIGHT},
        {enumKeyCodes::KEY_B, enumKeyCodes::CONTROLLER_B},
        {enumKeyCodes::KEY_A, enumKeyCodes::CONTROLLER_A},
        {enumKeyCodes::KEY_Enter, enumKeyCodes::CONTROLLER_Start}};

    if (m_eePos >= sequence.size())
        return CCLayer::keyDown(code);

    if (m_eePos < 3) {
        CCLayer::keyDown(code);
    }

    const auto &validCodes = sequence[m_eePos];
    if (std::find(validCodes.begin(), validCodes.end(), code) !=
        validCodes.end()) {
        m_eePos++;
    } else {
        m_eePos = 0;
        log::info("Reset");
        return;
    }

    if (m_eePos == sequence.size()) {
        onKonami();
    }
}

// mobile support lmao

void ServerListLayer::registerWithTouchDispatcher() {
  cocos2d::CCTouchDispatcher::get()->addTargetedDelegate(this, 10, false);
}

bool ServerListLayer::ccTouchBegan(cocos2d::CCTouch *touch,
                                   cocos2d::CCEvent *evt) {
  return true;
}

enum class Input { None, Up, Down, Left, Right, B, A, Start };

void ServerListLayer::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *evt) {
    static const std::vector<Input> sequence = {
        Input::Up,   Input::Up,    Input::Down, Input::Down,
        Input::Left, Input::Right, Input::Left, Input::Right,
        Input::B,    Input::A,     Input::Start};
    if (m_eePos >= sequence.size())
        return;

    auto diff = touch->getLocation() - touch->getStartLocation();
    auto axis = diff.normalize();
    Input inp = Input::None;
    if (sqrtf(diff.x * diff.x + diff.y * diff.y) < 50.f) {
        auto screenWidth = CCDirector::get()->getWinSize().width;
        auto tapLoc = touch->getLocation();
        if (tapLoc.x < screenWidth / 3) {
          inp = Input::A;
        } else if (tapLoc.x > screenWidth * 2 / 3) {
          inp = Input::B;
        } else {
          inp = Input::Start;
        }
    } else if (axis.y < -.75) {
        inp = Input::Down;
    } else if (axis.y > .75) {
        inp = Input::Up;
    } else if (axis.x < -.75) {
        inp = Input::Left;
    } else if (axis.x > .75) {
        inp = Input::Right;
    }

    if (sequence[m_eePos] == inp) {
        m_eePos++;
    } else {
        m_eePos = 0;
        return;
    }

    if (m_eePos == sequence.size()) {
        onKonami();
    }
}

void ServerListLayer::onKonami() {
    auto spr = CCSprite::createWithSpriteFrameName("geode.loader/settings.png");
    auto circle = CircleButtonSprite::create(spr, CircleBaseColor::Green, CircleBaseSize::Big);
    circle->setScale(.7f);
    auto btn = CCMenuItemSpriteExtra::create(
        circle, this, menu_selector(ServerListLayer::onSettings)
    );
    btn->setID("settings-button");
    m_bottomMenu->addChild(btn);
    m_bottomMenu->updateLayout();

    Mod::get()->setSavedValue("secret-settings", true);
    auto exp = CCParticleExplosion::create();
    exp->setStartColor({255,127, 0, 255});
    exp->setEndColorVar({255,127, 0, 255});
    exp->setEndColor({ 255, 0, 0, 255 });
    exp->setEndColorVar({ 255, 0, 0, 255 });
    exp->setSpeed(90);
    exp->setLife(.01f);
    exp->setEndSize(0.f);
    exp->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(7.5f), CallFuncExt::create([this, exp](){exp->removeFromParent();})));
    exp->setPosition(m_bottomMenu->convertToWorldSpace(btn->getPosition()));
    addChild(exp);
}

void ServerListLayer::onExit() {
    CCLayer::onExit();
    CCTouchDispatcher::get()->removeDelegate(this);
}

void ServerListLayer::onSettings(CCObject *sender) {
    FLAlertLayer::create("Sorry!", "The bonus settings aren't implemented yet. Check back later!", "Ok")->show();
}