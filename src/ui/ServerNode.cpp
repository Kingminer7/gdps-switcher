#include "ServerNode.hpp"
#include "GUI/CCControlExtension/CCScale9Sprite.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
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

    auto label = cocos2d::CCLabelBMFont::create(server.name.c_str(), "bigFont.fnt");
    label->limitLabelWidth(size.width - 50, .7f, 0.f);
    label->setAnchorPoint({0.f, 0.5f});
    addChildAtPosition(label, geode::Anchor::TopLeft, {8, -1 - label->getContentHeight() / 2});

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