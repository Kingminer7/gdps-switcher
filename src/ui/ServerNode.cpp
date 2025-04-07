#include "ServerNode.hpp"
#include "../utils/GDPSMain.hpp"
#include "GUI/CCControlExtension/CCScale9Sprite.h"
#include "Geode/binding/CCMenuItemSpriteExtra.hpp"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/ui/Layout.hpp"

using namespace GDPSTypes;

bool ServerNode::init(Server server, cocos2d::CCSize size) {
    if (!CCNode::init()) return false;
    m_server = server;
    this->setID(fmt::format("server-node-{}", server.name));
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

    m_menu = cocos2d::CCMenu::create();
    m_menu->setID("button-menu");
    m_menu->setContentSize({size.width / 2 - 8, size.height});
    m_menu->setLayout(geode::RowLayout::create()->setAxisAlignment(geode::AxisAlignment::End)->setAxisReverse(true));
    m_menu->setAnchorPoint({1.f, 0.5f});
    this->addChildAtPosition(m_menu, geode::Anchor::Right, {-8, 0});

    auto editSpr = ButtonSprite::create("Use");
    editSpr->setScale(.7f);
    if (GDPSMain::get()->getServer() == server) {
      editSpr->updateBGImage("GJ_button_05.png");
    } else {
      editSpr->updateBGImage("GJ_button_04.png");
    }
    auto editBtn = CCMenuItemSpriteExtra::create(editSpr, this, menu_selector(ServerNode::onSelect));
    editBtn->setID("select-btn");
    m_menu->addChild(editBtn);
    m_menu->updateLayout();

    return true;
};

ServerNode *ServerNode::create(Server server, cocos2d::CCSize size) {
    auto ret = new ServerNode;
    if (ret && ret->init(server, size)) {
      ret->autorelease();
      return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void ServerNode::onSelect(CCObject *sender) {

}