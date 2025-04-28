#include "ServerNode.hpp"
#include "utils/GDPSMain.hpp"
#include "utils/ServerInfoManager.hpp"
#include "ModifyServerPopup.hpp"

bool ServerNode::init(GDPSTypes::Server server, CCSize size, ServerListLayer *list) {
    if (!CCNode::init()) return false;
    this->m_listLayer = list;
    this->m_server = server;
    this->setID("server-node");
    this->m_obContentSize = size;
    this->setAnchorPoint({.5f, .5f});

    auto bg = CCScale9Sprite::create("GJ_square07.png", {0, 0, 80, 80});
    bg->setContentSize(size);
    bg->setID("background");
    this->addChildAtPosition(bg, Anchor::Center);

    auto nameLab = CCLabelBMFont::create(server.name.c_str(), "bigFont.fnt");
    nameLab->setID("name");
    nameLab->limitLabelWidth(size.width - 50, .7f, 0.f);
    nameLab->setAnchorPoint({0.f, 0.5f});
    this->addChildAtPosition(nameLab, Anchor::TopLeft, {8,  1- nameLab->getContentHeight() / 2});

    // TODO: maybe use prevter's label
    auto motdArea = MDTextArea::create(server.motd, {250.f, 37.5f});
    motdArea->setID("motd");
    motdArea->getScrollLayer()->setTouchEnabled(false);
    motdArea->getScrollLayer()->setMouseEnabled(false);
    motdArea->setAnchorPoint({0.f, 1.f});
    this->addChildAtPosition(motdArea, Anchor::Left, {14.f, 9.f});
    
    ServerInfoManager::get()->getInfoForServer(server, motdArea);

    m_menu = CCMenu::create();
    m_menu->setID("button-menu");
    m_menu->setContentSize({size.width / 2 - 4, size.height});
    // Whoever made ignoreInvisibleChildren return void, why.x
    auto layout = RowLayout::create()
        ->setAxisAlignment(AxisAlignment::End)
        ->setAxisReverse(true)
        ->setGap(2.f);
    layout->ignoreInvisibleChildren(true);
    m_menu->setLayout(layout);
    m_menu->setAnchorPoint({1.f, 0.5f});
    this->addChildAtPosition(m_menu, Anchor::Right, {-6, 0});

    auto useSpr = ButtonSprite::create("Use", "bigFont.fnt", list->m_selectedServer == server.id ? "GJ_button_02.png" : "GJ_button_01.png");
    useSpr->setScale(.6f);
    auto useBtn = CCMenuItemSpriteExtra::create(useSpr, this, menu_selector(ServerNode::onSelect));
    useBtn->setID("use-btn");
    useSpr->setCascadeOpacityEnabled(true);
    useBtn->setEnabled(list->m_selectedServer != server.id);
    m_menu->addChild(useBtn);

    auto editSpr = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName("edit.png"_spr), EditorBaseColor::Green);
    editSpr->setScale(.75f);
    auto editBtn = CCMenuItemSpriteExtra::create(
        editSpr,
        this,
        menu_selector(ServerNode::onEdit)
    );
    editBtn->setVisible(false);
    editBtn->setID("edit-btn");

    auto deleteSpr = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"), EditorBaseColor::BrightGreen);
    deleteSpr->setScale(.75f);
    if (auto x = deleteSpr->getChildByType<CCSprite *>(0)) {
        x->setScale(x->getScale() * .9);
    }
    auto deleteBtn = CCMenuItemSpriteExtra::create(
        deleteSpr,
        this,
        menu_selector(ServerNode::onDelete)
    );
    deleteBtn->setID("delete-btn");
    deleteBtn->setVisible(false);

    m_menu->addChild(deleteBtn);
    m_menu->addChild(editBtn);

    m_menu->updateLayout();

    return true;
};

ServerNode *ServerNode::create(GDPSTypes::Server server, CCSize size, ServerListLayer *list) {
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

void ServerNode::updateSelected(GDPSTypes::Server server) {
  auto btn = static_cast<CCMenuItemSpriteExtra *>(m_menu->getChildByID("use-btn"));
  if (!btn) return;
  auto spr = btn->getChildByType<ButtonSprite *>(0);
  if (!spr) return;
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

void ServerNode::updateInfo() {
  log::info("{}", m_server.name);
}

GDPSTypes::Server ServerNode::getServer() {
  return m_server;
}

void ServerNode::onEdit(CCObject *sender) {
  if (m_locked) return;
  ModifyServerPopup::create(m_server, m_listLayer)->show();
}

void ServerNode::onDelete(CCObject *sender) {
  if (m_locked) return;
  createQuickPopup("Delete Server", fmt::format("Are you sure you want to delete {}?", m_server.name), "No", "Yes", [this](auto, bool second) {
      if (second) {
          GDPSMain::get()->m_servers.erase(m_server.id);
          m_listLayer->updateList();
      }
  });
}

void ServerNode::setEditing(bool editing) {
  m_editing = editing;
  auto editBtn = m_menu->getChildByID("edit-btn");
  auto deleteBtn = m_menu->getChildByID("delete-btn");
  auto useBtn = m_menu->getChildByID("use-btn");
  if (editBtn) editBtn->setVisible(m_editing && !m_locked);
  if (deleteBtn) deleteBtn->setVisible(m_editing && !m_locked);
  if (useBtn) useBtn->setVisible(!m_editing);
  m_menu->updateLayout();
}

bool ServerNode::isEditing() {
  return m_editing;
}
