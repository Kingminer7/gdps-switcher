#include "ServerNode.hpp"
#include "utils/GDPSMain.hpp"
#include "utils/ServerInfoManager.hpp"

bool ServerNode::init(GDPSTypes::Server server, CCSize size, ServerListLayer *list, bool editMode) {
  if (!DragNode::init(static_cast<DragLayer*>(list->m_scroll->m_contentLayer))) return false;
    m_editing = editMode;
    this->m_listLayer = list;
    this->m_server = server;
    this->setID("server-node");
    this->m_obContentSize = size;
    ignoreAnchorPointForPosition(false);
    setAnchorPoint({.5f, .5f});
    queueInMainThread([this](){
      CCTouchDispatcher::get()->setPriority(-129, this);
    });

    auto bg = CCScale9Sprite::create("GJ_square07.png", {0, 0, 80, 80});
    bg->setContentSize(size);
    bg->setID("background");
    this->addChildAtPosition(bg, Anchor::Center, {}, false);

    auto nameLab = CCLabelBMFont::create(server.name.c_str(), "bigFont.fnt");
    nameLab->setID("name");
    nameLab->limitLabelWidth(size.width - 50, .7f, 0.f);
    nameLab->setAnchorPoint({0.f, 0.5f});
    this->addChildAtPosition(nameLab, Anchor::TopLeft, {8, -1 - nameLab->getContentHeight() / 2}, false);

    m_menu = CCMenu::create();
    m_menu->setID("button-menu");
    m_menu->setContentSize({size.width / 2 - 4, size.height});
    m_menu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::End)->setAxisReverse(true));
    m_menu->setAnchorPoint({1.f, 0.5f});
    this->addChildAtPosition(m_menu, Anchor::Right, {-6, 0}, false);

    if (!editMode) {
      // TODO: maybe use prevter's label
      auto motdArea = MDTextArea::create(server.motd, {250.f, 37.5f});
      motdArea->setID("motd");
      motdArea->getScrollLayer()->setTouchEnabled(false);
      motdArea->getScrollLayer()->setMouseEnabled(false);
      motdArea->setAnchorPoint({0.f, 1.f});
      this->addChildAtPosition(motdArea, Anchor::Left, {8.f, 9.f}, false);
      ServerInfoManager::get()->getInfoForServer(server, motdArea);

      auto useSpr = ButtonSprite::create("Use", "bigFont.fnt", list->m_selectedServer == server ? "GJ_button_02.png" : "GJ_button_01.png");
      useSpr->setScale(.6f);
      auto useBtn = CCMenuItemSpriteExtra::create(useSpr, this, menu_selector(ServerNode::onSelect));
      useBtn->setID("select-btn");
      useSpr->setCascadeOpacityEnabled(true);
      useBtn->setEnabled(list->m_selectedServer != server);
      m_menu->addChild(useBtn);
    }
    m_menu->updateLayout();

    return true;
};

ServerNode *ServerNode::create(GDPSTypes::Server server, CCSize size, ServerListLayer *list, bool editMode) {
    auto ret = new ServerNode;
    if (ret && ret->init(server, size, list, editMode)) {
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

bool ServerNode::isEditNode() {
  return m_editing;
}