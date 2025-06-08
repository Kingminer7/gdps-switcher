#include "ServerNode.hpp"
#include "utils/GDPSMain.hpp"
#include "ModifyServerPopup.hpp"

#include <Geode/ui/LazySprite.hpp>

bool ServerNode::init(GDPSTypes::Server& server, CCSize size, ServerListLayer *list, bool odd) {
    if (!CCNode::init()) return false;
    this->m_listLayer = list;
    this->m_server = server;
    this->setID("server-node");
    this->m_obContentSize = size;
    this->setAnchorPoint({.5f, .5f});

    auto bg = CCLayerColor::create(odd ? ccColor4B{0, 0, 0, 60} : ccColor4B{0, 0, 0, 30}, size.width, size.height);
    bg->setID("background");
    bg->ignoreAnchorPointForPosition(false);
    this->addChildAtPosition(bg, Anchor::Center);
    auto icon = LazySprite::create({size.height - 20, size.height - 20}, !server.icon.empty());
    icon->setAutoResize(true);
    icon->setScale(1.f);
    icon->setLoadCallback([icon](Result<> status) {
        if (status.isErr()) {
            icon->initWithSpriteFrameName("grayGdLogo.png"_spr);
        }
    });
    icon->setID("icon");
    this->addChildAtPosition(icon, Anchor::Left, {7.5f + size.height / 2 - 10, 0});
    updateInfo(server);

    auto nameLab = CCLabelBMFont::create(server.name.c_str(), "bigFont.fnt");
    nameLab->setID("name");
    nameLab->limitLabelWidth(size.width - 50, .7f, 0.f);
    nameLab->setAnchorPoint({0.f, 0.5f});
    this->addChildAtPosition(nameLab, Anchor::TopLeft, {69,  1- nameLab->getContentHeight() / 2});

    // TODO: maybe use prevter's label
    auto motdArea = MDTextArea::create(server.motd, {230.f, 37.5f});
    motdArea->setID("motd");
    motdArea->getScrollLayer()->setTouchEnabled(false);
    motdArea->getScrollLayer()->setMouseEnabled(false);
    motdArea->setAnchorPoint({0.f, 1.f});
    this->addChildAtPosition(motdArea, Anchor::Left, {75.f, 9.f});

    m_menu = CCMenu::create();
    m_menu->setID("button-menu");
    m_menu->setContentSize({size.width / 2 - 4, size.height});
    // Whoever made ignoreInvisibleChildren return void, why.x
    auto layout = RowLayout::create()
        ->setAxisAlignment(AxisAlignment::End)
        ->setAxisReverse(true)
        ->setGap(3.3f);
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

    auto editSpr = CCSprite::create("GJ_button_04.png");
    editSpr->setScale(.5475f);
    auto pencilSpr = CCSprite::createWithSpriteFrameName("edit.png"_spr);
    pencilSpr->setScale(1.3f);
    editSpr->addChildAtPosition(pencilSpr, Anchor::Center);
    auto editBtn = CCMenuItemSpriteExtra::create(
        editSpr,
        this,
        menu_selector(ServerNode::onEdit)
    );
    editBtn->setVisible(false);
    editBtn->setID("edit-btn");

    auto deleteSpr = CCSprite::create("GJ_button_06.png");
    deleteSpr->setScale(.5475f);
    auto xSpr = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
    xSpr->setScale(1.2);
    deleteSpr->addChildAtPosition(xSpr, Anchor::Center);
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

ServerNode *ServerNode::create(GDPSTypes::Server& server, CCSize size, ServerListLayer *list, bool odd) {
    auto ret = new ServerNode;
    if (ret && ret->init(server, size, list, odd)) {
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
    } else {
        spr->updateBGImage("GJ_button_01.png");
        btn->setEnabled(true);
    }
    // For some reason the CCMenuItemSpriteExtra gets changed so i gotta fix it... we love robtop code
    btn->setContentSize(spr->getScaledContentSize());   
    spr->setPosition(btn->getContentSize() / 2);
}

void ServerNode::updateInfo(LoadDataEvent event) {
    if (auto nameLab = static_cast<CCLabelBMFont*>(this->getChildByID("name"))) {
        nameLab->setString(m_server.name.c_str());
        nameLab->limitLabelWidth(this->m_obContentSize.width - 50, .7f, 0.f);
    }

    if (auto motdArea = static_cast<MDTextArea *>(this->getChildByID("motd"))) {
        motdArea->setString(m_server.motd.c_str());
    }

    if (auto icon = static_cast<LazySprite *>(this->getChildByID("icon"))) {
        icon->removeAllChildren();
        log::info("Icon: {}", m_server.icon);
        log::info("Is sprite: {}", m_server.iconIsSprite);
        if (server.iconIsSprite) {
            auto frame = CCSpriteFrameCache::get()->spriteFrameByName(m_server.icon.c_str());
            if (frame) {
                icon->setDisplayFrame(frame);
                icon->setScale((this->getContentHeight() - 20) / icon->getContentHeight());
                icon->setAutoResize(true);
            } else {
                auto frame = CCSpriteFrameCache::get()->spriteFrameByName("grayGdLogo.png"_spr);
                if (frame) {
                    icon->setDisplayFrame(frame);
                    icon->setScale((this->getContentHeight() - 20) / icon->getContentHeight());
                    icon->setAutoResize(true);
                }
            }
        } else if (server.icon.empty()) {
            auto frame = CCSpriteFrameCache::get()->spriteFrameByName("grayGdLogo.png"_spr);
            if (frame) {
                icon->setDisplayFrame(frame);
                icon->setScale((this->getContentHeight() - 20) / icon->getContentHeight());
                icon->setAutoResize(true);
            }
        } else {
            icon->loadFromUrl(server.icon);
        }
    }

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
          GDPSMain::get()->save();
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
