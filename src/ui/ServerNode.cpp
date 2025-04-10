#include "ServerNode.hpp"
#include "../utils/GDPSMain.hpp"
#include "GUI/CCControlExtension/CCScale9Sprite.h"
#include "Geode/binding/ButtonSprite.hpp"
#include "Geode/binding/CCMenuItemSpriteExtra.hpp"
#include "Geode/binding/MultilineBitmapFont.hpp"
#include "Geode/binding/TextArea.hpp"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/ui/Layout.hpp"
#include "Geode/ui/MDTextArea.hpp"
#include "utils/ServerInfoManager.hpp"

using namespace GDPSTypes;

bool ServerNode::init(Server server, cocos2d::CCSize size, ServerListLayer *list) {
    if (!CCNode::init()) return false;
    this->m_listLayer = list;
    this->m_server = server;
    this->setID(fmt::format("server-node", server.name));
    this->m_obContentSize = size;
    this->setAnchorPoint({.5f, .5f});

    auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square07.png", {0, 0, 80, 80});
    bg->setContentSize(size);
    bg->setID("background");
    this->addChildAtPosition(bg, geode::Anchor::Center);

    auto nameLab = cocos2d::CCLabelBMFont::create(server.name.c_str(), "bigFont.fnt");
    nameLab->setID("name");
    nameLab->limitLabelWidth(size.width - 50, .7f, 0.f);
    nameLab->setAnchorPoint({0.f, 0.5f});
    this->addChildAtPosition(nameLab, geode::Anchor::TopLeft, {8, -1 - nameLab->getContentHeight() / 2});

    // auto motdArea = TextArea::create(
    //   server.motd, 
    //   "chatFont.fnt", 
    //   .7f, 
    //   250.f, 
    //   {0.f, 0.f}, 
    //   13.f, 
    //   false
    // );
    // TODO: maybe use prevter's label
    auto motdArea = geode::MDTextArea::create(server.motd, {250.f, 37.5f});
    motdArea->setID("motd");
    motdArea->getScrollLayer()->setTouchEnabled(false);
    motdArea->getScrollLayer()->setMouseEnabled(false);
    motdArea->setAnchorPoint({0.f, 1.f});
    this->addChildAtPosition(motdArea, geode::Anchor::Left, {8.f, 9.f});
    
    ServerInfoManager::get()->getInfoForServer(server, motdArea);

    m_menu = cocos2d::CCMenu::create();
    m_menu->setID("button-menu");
    m_menu->setContentSize({size.width / 2 - 4, size.height});
    m_menu->setLayout(geode::RowLayout::create()->setAxisAlignment(geode::AxisAlignment::End)->setAxisReverse(true));
    m_menu->setAnchorPoint({1.f, 0.5f});
    this->addChildAtPosition(m_menu, geode::Anchor::Right, {-6, 0});

    auto useSpr = ButtonSprite::create("Use", "bigFont.fnt", list->m_selectedServer == server ? "GJ_button_02.png" : "GJ_button_01.png");
    useSpr->setScale(.6f);
    auto useBtn = CCMenuItemSpriteExtra::create(useSpr, this, menu_selector(ServerNode::onSelect));
    useBtn->setID("select-btn");
    useSpr->setCascadeOpacityEnabled(true);
    useBtn->setEnabled(list->m_selectedServer != server);
    m_menu->addChild(useBtn);
    m_menu->updateLayout();

    return true;
};

ServerNode *ServerNode::create(Server server, cocos2d::CCSize size, ServerListLayer *list) {
    auto ret = new ServerNode;
    if (ret && ret->init(server, size, list)) {
      ret->autorelease();
      return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void ServerNode::onSelect(CCObject *sender) {
    m_listLayer->onSelect(m_server);
}

void ServerNode::updateVisual(GDPSTypes::Server server) {
  auto btn = static_cast<CCMenuItemSpriteExtra *>(m_menu->getChildByID("select-btn"));
  auto spr = btn->getChildByType<ButtonSprite *>(0);
  if (server == m_server) {
    spr->updateBGImage("GJ_button_02.png");
    btn->setEnabled(false);
    // spr->setOpacity(127);
  } else {
    spr->updateBGImage("GJ_button_01.png");
    btn->setEnabled(true);
    // spr->setOpacity(255);
  }
}

GDPSTypes::Server ServerNode::getServer() {
  return m_server;
}