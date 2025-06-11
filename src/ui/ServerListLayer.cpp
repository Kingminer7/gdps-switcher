#include "ServerListLayer.hpp"
#include "ModifyServerPopup.hpp"
#include "ServerNode.hpp"
#include "BonusSettingsPopup.hpp"
#include "Types.hpp"
#include "utils/GDPSMain.hpp"
#include "utils/ServerInfoManager.hpp"

#include <km7dev.server_api/include/ServerAPIEvents.hpp>

using namespace geode::prelude;

int ServerListLayer::m_selectedServer = -1;

bool ServerListLayer::init() {
    if (!CCLayer::init()) return false;

    bool switcherTheme = Mod::get()->getSavedValue<bool>("ss-nintendo-switcher");

    registerWithTouchDispatcher();

    if (m_selectedServer == -1) m_selectedServer = GDPSMain::get()->m_currentServer;

    this->setID("ServerListLayer"_spr);
    this->setKeyboardEnabled(true);
    this->setKeypadEnabled(true);

    auto bg = geode::createLayerBG();
    if (switcherTheme) bg->setColor({0, 228, 255});
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
    editSpr->setZOrder(1);
    editSpr->setID("edit-sprite");
    auto editCircle = CircleButtonSprite::create(editSpr, CircleBaseColor::Green, CircleBaseSize::Big);
    editSpr->setScale(1.4f);
    editCircle->setID("edit-circle");
    editCircle->setScale(.7f);
    auto blueCircle = CCSprite::createWithSpriteFrameName("geode.loader/baseCircle_Big_Blue.png");
    blueCircle->setID("blue-circle");
    blueCircle->setVisible(false);
    editCircle->addChildAtPosition(blueCircle, geode::Anchor::Center, {0.f, 0.f}, false);
    auto editBtn = CCMenuItemSpriteExtra::create(
        editCircle, this, menu_selector(ServerListLayer::onEdit)
    );
    editBtn->setID("edit-button");
    m_bottomMenu->addChild(editBtn);

    //auto gplusSignInBtn = CCMenuItemExt::createSpriteExtraWithFrameName("gplusSignIn_001.png", 1.f, [this](auto) {
    //    for (auto server : std::ranges::views::values(m_servers)) {
    //        ServerInfoManager::get()->fetch(server);
    //    }
    //});
    //m_bottomMenu->addChild(gplusSignInBtn);

    m_bottomMenu->setLayout(
        geode::ColumnLayout::create()
            ->setAxisAlignment(geode::AxisAlignment::Start)
            ->setAxisReverse(false)
            ->setGap(3.f)
    );
    this->addChildAtPosition(m_bottomMenu, geode::Anchor::BottomRight, ccp(-25.f, 25.f / 4.f), false);
    
    auto scrollFrame = CCLayerColor::create(switcherTheme ? ccColor4B{255, 0, 0, 255} : ccColor4B{191, 114, 62, 255}, 356, 220);
    scrollFrame->setID("server-frame");
    scrollFrame->ignoreAnchorPointForPosition(false);
    addChildAtPosition(scrollFrame, geode::Anchor::Center, {0.f, 0.f}, false);

    auto top = CCSprite::createWithSpriteFrameName("GJ_table_top_001.png");
    scrollFrame->addChildAtPosition(top, geode::Anchor::Top, {0.f, 15.5});
    top->setZOrder(1);
    top->setID("top");
    if (switcherTheme) top->setColor({255, 0, 0});

    auto right = CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
    right->setFlipX(true);
    right->setScaleY(3.438f); 
    scrollFrame->addChildAtPosition(right, geode::Anchor::Right, {6.f, 0.f});
    right->setZOrder(1);
    right->setID("right");
    if (switcherTheme) right->setColor({255, 0, 0});

    auto left = CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
    left->setScaleY(3.438f);
    scrollFrame->addChildAtPosition(left, geode::Anchor::Left, {-6.f, 0.f});
    left->setZOrder(1);
    left->setID("left");
    if (switcherTheme) left->setColor({255, 0, 0});

    auto bottom = CCSprite::createWithSpriteFrameName("GJ_table_bottom_001.png");
    bottom->setID("bottom");
    scrollFrame->addChildAtPosition(bottom, geode::Anchor::Bottom, {0.f, -13.f});
    bottom->setZOrder(1);
    if (switcherTheme) bottom->setColor({255, 0, 0});

    auto title = CCLabelBMFont::create(switcherTheme ? "Nintendo Switcher" : "Private Servers", "bigFont.fnt");
    title->setScale(.8f);
    title->setID("title");
    title->setZOrder(2);
    scrollFrame->addChildAtPosition(title, geode::Anchor::Top, {0.f, 19.5f});

    m_scroll = geode::ScrollLayer::create({356, 220});
    m_scroll->setID("server-scroll");
    m_scroll->ignoreAnchorPointForPosition(false);
    m_scroll->m_contentLayer->setLayout(
        SimpleColumnLayout::create()
            ->setMainAxisDirection(AxisDirection::TopToBottom)
            ->setMainAxisAlignment(MainAxisAlignment::Start)
            ->setMainAxisScaling(AxisScaling::Grow)
    );
    // addChildAtPosition(m_scroll, geode::Anchor::Center, { 0.f, 0.f}, false);

    auto clip = cocos2d::CCClippingNode::create();
    clip->setID("server-list");
    clip->setZOrder(0);
    clip->setContentSize({356, 220});
    clip->setAnchorPoint({0.5, 0.5});
    clip->setAlphaThreshold(0.05f);
    clip->addChildAtPosition(m_scroll, geode::Anchor::Center);
    scrollFrame->addChildAtPosition(clip, geode::Anchor::Center, {0.f, 0.f}, false);

    auto stencil = CCSprite::create("square.png");
    stencil->setScaleX(356 / stencil->getContentWidth());
    stencil->setScaleY(220 / stencil->getContentHeight());
    stencil->setPosition({178, 110});
    clip->setStencil(stencil);

    updateList();

    m_scroll->scrollToTop();

    if (Mod::get()->getSavedValue("secret-settings", false)) {
        m_konamiPos = 12;

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

    static bool fetched = false;
    if (!fetched) {
        fetched = true;
        for (auto [id, server] : GDPSMain::get()->m_servers) {
            ServerInfoManager::get()->fetch(server);
        }
    }

    return true;
}

void ServerListLayer::updateList() {
    auto order = Mod::get()->getSavedValue<std::vector<int>>("server-order", {});
    for (auto server : std::ranges::views::values(GDPSMain::get()->m_servers)) {
        if (server.id < 0 && server.id != -2) continue;
        if (std::find(order.begin(), order.end(), server.id) == order.end()) {
            order.push_back(server.id);
        }
    }
    Mod::get()->setSavedValue("server-order", order);

    m_servers = GDPSMain::get()->m_servers;
    m_scroll->m_contentLayer->removeAllChildren();
    bool odd = false;
    for (int id : order) {
        auto it = m_servers.find(id);
        if (it == m_servers.end()) continue;
        odd = !odd;
        auto& server = it->second;
        auto node = ServerNode::create(server, {356, 55}, this, odd);
        if (server.id == -2) {
            node->m_locked = true;
        }
        node->setEditing(m_isEditing);
        m_scroll->m_contentLayer->addChild(node);
    }
    m_scroll->m_contentLayer->updateLayout();
    // m_scroll->scrollToTop();
}

void ServerListLayer::keyBackClicked() {
    this->onBack(nullptr);
}

void ServerListLayer::onBack(cocos2d::CCObject *sender) {
    if (m_selectedServer != GDPSMain::get()->m_currentServer) {
        geode::createQuickPopup("GDPS Switcher", "Switching servers requires a restart.", "Restart later", "Restart now", [this](auto, bool now) {
            if (now) {
                geode::utils::game::restart(true);
            } else {
                cocos2d::CCDirector::get()->popSceneWithTransition(.5f, kPopTransitionFade);
            }
        });
    } else {
        cocos2d::CCDirector::get()->popSceneWithTransition(.5f, kPopTransitionFade);
    }
}

ServerListLayer *ServerListLayer::create() {
    auto ret = new ServerListLayer;
    if (ret->init()) {
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

void ServerListLayer::onSelect(const GDPSTypes::Server &server) const {
    m_selectedServer = server.id;
    for (auto node : CCArrayExt<ServerNode>(m_scroll->m_contentLayer->getChildren())) {
        if (!node) return;
        node->updateSelected(server);
        Mod::get()->setSavedValue("current", server.id);
    }
}

void ServerListLayer::onAdd(CCObject *sender) {
    int id = 0;
    for (const auto &serverId: m_servers | std::views::keys) {
        if (serverId < 0) continue;
        if (serverId == id) id++;
        else break;
    }
    ModifyServerPopup::create({id, "", "", ""}, this)->show();
}

void ServerListLayer::onEdit(CCObject *sender) {
    m_isEditing = !m_isEditing;
    if (auto editBtn = m_bottomMenu->getChildByID("edit-button")) {
        if (auto editSpr = editBtn->getChildByID("edit-circle")) {
            if (auto blueCircle = editSpr->getChildByID("blue-circle")) {
                blueCircle->setVisible(m_isEditing);
            }
        }
    }

    for (auto node : CCArrayExt<ServerNode>(m_scroll->m_contentLayer->getChildren())) {
        if (!node) return;
        node->setEditing(m_isEditing);
    }
}

void ServerListLayer::onSettings(CCObject *sender) {
    BonusSettingsPopup::create()->show();
    // FLAlertLayer::create("Sorry!", "The bonus settings aren't implemented yet. Check back later!", "Ok")->show();
}


//region Konami code easter egg

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

    if (m_konamiPos >= sequence.size())
        return CCLayer::keyDown(code);

    if (m_konamiPos < 3) {
        CCLayer::keyDown(code);
    }

    const auto &validCodes = sequence[m_konamiPos];
    if (std::find(validCodes.begin(), validCodes.end(), code) !=
        validCodes.end()) {
        m_konamiPos++;
    } else {
        m_konamiPos = 0;
        return;
    }

    if (m_konamiPos == sequence.size()) {
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
    if (m_konamiPos >= sequence.size())
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

    if (sequence[m_konamiPos] == inp) {
        m_konamiPos++;
    } else {
        m_konamiPos = 0;
        return;
    }

    if (m_konamiPos == sequence.size()) {
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
//endregion
