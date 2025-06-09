#include "ServerNode.hpp"
#include "utils/GDPSMain.hpp"
#include "ModifyServerPopup.hpp"

#include <Geode/ui/LazySprite.hpp>

bool ServerNode::init(CCSize size, ServerListLayer *list, bool odd) {
    if (!CCNode::init()) return false;

    m_listener.bind([this](LoadDataEvent *event) {
        updateInfo();
        return ListenerResult::Propagate;
    });

    this->m_listLayer = list;
    this->setID("server-node");
    this->m_obContentSize = size;
    this->setAnchorPoint({.5f, .5f});

    auto bg = CCLayerColor::create(odd ? ccColor4B{0, 0, 0, 60} : ccColor4B{0, 0, 0, 30}, size.width, size.height);
    bg->setID("background");
    bg->ignoreAnchorPointForPosition(false);
    this->addChildAtPosition(bg, Anchor::Center);

    auto nameLab = CCLabelBMFont::create(m_server.name.c_str(), "bigFont.fnt");
    nameLab->setID("name");
    nameLab->limitLabelWidth(size.width - 120, .6f, 0.f);
    nameLab->setAnchorPoint({0.f, 1.f});
    this->addChildAtPosition(nameLab, Anchor::TopLeft, {60, -2});

    updateInfo();

    m_menu = CCMenu::create();
    m_menu->setID("button-menu");
    m_menu->setContentSize({size.width - 306, size.height});
    auto layout = RowLayout::create()
        ->setAxisAlignment(AxisAlignment::End)
        ->setAxisReverse(true)
        ->setGap(3.3f)
        ->setCrossAxisOverflow(true)
        ->setGrowCrossAxis(true)
        ->setCrossAxisAlignment(AxisAlignment::Center);
    layout->ignoreInvisibleChildren(true);
    m_menu->setLayout(layout);
    m_menu->setAnchorPoint({1.f, 0.5f});
    this->addChildAtPosition(m_menu, Anchor::Right, {-8, 0});

    auto useSpr = ButtonSprite::create("Use", "bigFont.fnt", list->m_selectedServer == m_server.id ? "GJ_button_02.png" : "GJ_button_01.png");
    useSpr->setScale(.6f);
    auto useBtn = CCMenuItemSpriteExtra::create(useSpr, this, menu_selector(ServerNode::onSelect));
    useBtn->setID("use-btn");
    useSpr->setCascadeOpacityEnabled(true);
    useBtn->setEnabled(list->m_selectedServer != m_server.id);
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

    auto upSpr = CCSprite::create("GJ_button_05.png");
    upSpr->setScale(0.5475f);
    auto upArrowSpr = CCSprite::createWithSpriteFrameName("d_arrow_03_001.png");
    upArrowSpr->setScale(0.6f);
    upArrowSpr->setRotation(-90.f);
    upSpr->addChildAtPosition(upArrowSpr, Anchor::Center);
    auto upBtn = CCMenuItemSpriteExtra::create(
        upSpr,
        this,
        menu_selector(ServerNode::onMove)
    );
    upBtn->setID("up-btn");
    upBtn->setVisible(false);

    auto downSpr = CCSprite::create("GJ_button_05.png");
    downSpr->setScale(0.5475f);
    auto downArrowSpr = CCSprite::createWithSpriteFrameName("d_arrow_03_001.png");
    downArrowSpr->setScale(0.6f);
    downArrowSpr->setRotation(90.f);
    downSpr->addChildAtPosition(downArrowSpr, Anchor::Center);
    auto downBtn = CCMenuItemSpriteExtra::create(
        downSpr,
        this,
        menu_selector(ServerNode::onMove)
    );
    downBtn->setID("down-btn");
    downBtn->setVisible(false);

    m_menu->addChild(editBtn);
    m_menu->addChild(upBtn);
    m_menu->addChild(deleteBtn);
    m_menu->addChild(downBtn);

    m_menu->updateLayout();

    return true;
};

ServerNode *ServerNode::create(GDPSTypes::Server& server, CCSize size, ServerListLayer *list, bool odd) {
    auto ret = new ServerNode(server);
    if (ret && ret->init(size, list, odd)) {
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

void ServerNode::updateInfo() {
    m_server = GDPSMain::get()->m_servers[m_server.id];
    if (auto nameLab = static_cast<CCLabelBMFont*>(this->getChildByID("name"))) {
        nameLab->setString(m_server.name.c_str());
        nameLab->limitLabelWidth(this->m_obContentSize.width - 120, .7f, 0.f);
    }

    auto motdArea = static_cast<MDTextArea *>(this->getChildByID("motd"));
    if (!motdArea) {
        motdArea = MDTextArea::create(m_server.motd, {246.f, getContentHeight() - 27.f});
        motdArea->setID("motd");
	motdArea->getChildByType<CCScale9Sprite>(0)->setVisible(false);
        motdArea->getScrollLayer()->setTouchEnabled(false);
        motdArea->getScrollLayer()->setMouseEnabled(false);
        motdArea->setAnchorPoint({0.f, 0.f});
        this->addChildAtPosition(motdArea, Anchor::BottomLeft, {61.f, 4.5f});
    }
    motdArea->setString(m_server.motd.c_str());

    if (auto icon = this->getChildByID("icon")) {
        icon->removeFromParent();
    }

    CCSprite *icon;

    if (m_server.iconIsSprite) {
        icon = CCSprite::createWithSpriteFrameName(m_server.icon.c_str());
        if (icon && !icon->getUserObject("geode.texture-loader/fallback")) {
            icon->setScale((this->getContentHeight() - 10) / icon->getContentHeight());
        } else {
            icon = CCSpriteGrayscale::createWithSpriteFrameName("gdlogo.png"_spr);
            if (icon) {
                icon->setScale((this->getContentHeight() - 10) / icon->getContentHeight());
            }
        }
    } else if (m_server.icon.empty()) {
        icon = CCSpriteGrayscale::createWithSpriteFrameName("gdlogo.png"_spr);
        if (icon) {
            icon->setScale((this->getContentHeight() - 10) / icon->getContentHeight());
        } else {
            icon = CCSpriteGrayscale::createWithSpriteFrameName("gdlogo.png"_spr);
            if (icon) {
                icon->setScale((this->getContentHeight() - 10) / icon->getContentHeight());
            }
        }
    } else {
        auto ls = LazySprite::create({m_obContentSize.height - 10, m_obContentSize.height - 10}, !m_server.icon.empty());
        ls->setAutoResize(true);
        ls->setScale(1.f);
        ls->setLoadCallback([this, &icon, ls](Result<> status) {
            if (status.isErr()) {
               ls->removeFromParent();
	       icon = CCSpriteGrayscale::createWithSpriteFrameName("gdlogo.png"_spr
  );
              if (icon) {
                  icon->setScale((this->getContentHeight() - 10) / icon->getContentHeight());
icon->setID("icon");
this->addChildAtPosition(icon, Anchor::Left, {m_obContentSize.height / 2 + 2.5f, 0});
              }
            }
        });
        ls->loadFromUrl(m_server.icon);
        icon = ls;
    }

    if (icon) {
    icon->setID("icon");
    this->addChildAtPosition(icon, Anchor::Left, {m_obContentSize.height / 2 + 2.5f, 0});
    }
}

GDPSTypes::Server& ServerNode::getServer() {
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
    bool show = m_editing && !m_locked;
    if (auto btn = m_menu->getChildByID("edit-btn")) btn->setVisible(show);
    if (auto btn = m_menu->getChildByID("delete-btn")) btn->setVisible(show);
    auto order = Mod::get()->getSavedValue<std::vector<int>>("server-order", {});
    log::info("{}", order);
    log::info("{}", m_server.id);
    auto it = std::find(order.begin(), order.end(), m_server.id);
    if (auto btn = m_menu->getChildByID("up-btn")) {
        btn->setVisible(show);
        auto spr = btn->getChildByType<CCSprite*>(0);
        if (spr) {
            bool isSecond = it != order.end() && std::next(order.begin()) == it;
            spr->setVisible(!isSecond);
        }
    }
    if (auto btn = m_menu->getChildByID("down-btn")) {
        btn->setVisible(show);
        auto spr = btn->getChildByType<CCSprite*>(0);
        if (spr) {
            bool isLast = (it != order.end() && std::prev(order.end()) == it);
            spr->setVisible(!isLast);
        }
    }
    if (auto btn = m_menu->getChildByID("use-btn")) btn->setVisible(!m_editing);
    m_menu->updateLayout();
}

bool ServerNode::isEditing() {
    return m_editing;
}

void ServerNode::onMove(CCObject *sender) {
    if (m_locked) return;
    auto btn = static_cast<CCMenuItemSpriteExtra *>(sender);
    if (!btn) return;

    int dir = 0;
    if (btn->getID() == "up-btn") dir = -1;
    else if (btn->getID() == "down-btn") dir = 1;

    auto order = Mod::get()->getSavedValue<std::vector<int>>("server-order", {});
    auto it = std::find(order.begin(), order.end(), m_server.id);
    if (it == order.end()) return;

    int index = std::distance(order.begin(), it);
    int newIndex = index + dir;

    if (newIndex < 1 || newIndex >= static_cast<int>(order.size())) return;

    std::swap(order[index], order[newIndex]);
    Mod::get()->setSavedValue("server-order", order);
    m_listLayer->updateList();
}
