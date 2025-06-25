#include "ServerNode.hpp"
#include "utils/GDPSMain.hpp"
#include "ModifyServerPopup.hpp"
#include "ColorLabel.hpp"

#include <Geode/ui/LazySprite.hpp>

bool ServerNode::init(CCSize size, ServerListLayer *list, int index) {
    if (!CCNode::init()) return false;
    m_index = index;

    m_listener.bind([this](LoadDataEvent *event) {
        updateInfo();
        return ListenerResult::Propagate;
    });

    this->m_listLayer = list;
    this->setID("server-node");
    this->m_obContentSize = size;
    this->setAnchorPoint({.5f, .5f});

    ccColor4B color;
    if (Mod::get()->getSavedValue("ss-rainbow", false)) {
	switch(index % 6) {
	    case 0: color = ccc4(228, 3, 3, 150); break;
	    case 1: color = ccc4(255, 140, 0, 150); break;
	    case 2: color = ccc4(255, 237, 0, 150); break;
	    case 3: color = ccc4(0, 128, 38, 150); break;
	    case 4: color = ccc4(0, 77, 255, 150); break;
	    case 5: color = ccc4(117, 7, 135, 150); break;
	}
    } else {
        color = index % 2 == 0 ? ccColor4B{0, 0, 0, 60} : ccColor4B{0, 0, 0, 30};
    }

    auto bg = CCLayerColor::create(color, size.width, size.height);
    bg->setID("background");
    bg->ignoreAnchorPointForPosition(false);
    this->addChildAtPosition(bg, Anchor::Center);

    auto nameLab = CCLabelBMFont::create(m_server.name.c_str(), "bigFont.fnt");
    nameLab->setID("name");
    nameLab->limitLabelWidth(size.width - 150, .6f, 0.f);
    nameLab->setAnchorPoint({0.f, 0.f});
    this->addChildAtPosition(nameLab, Anchor::TopLeft, {60, -nameLab->getContentHeight()/2 - 8});

    updateInfo();

    m_editMenu = CCMenu::create();
    m_editMenu->setID("edit-menu");
    m_editMenu->setContentSize({55, size.height});
    m_editMenu->setVisible(false);
    auto layout = RowLayout::create()
        ->setAxisAlignment(AxisAlignment::End)
        ->setAxisReverse(true)
        ->setGap(3.3f)
        ->setCrossAxisOverflow(true)
        ->setGrowCrossAxis(true)
        ->setCrossAxisAlignment(AxisAlignment::Center);
    layout->ignoreInvisibleChildren(true);
    m_editMenu->setLayout(layout);
    m_editMenu->setAnchorPoint({1.f, 0.5f});
    this->addChildAtPosition(m_editMenu, Anchor::Right, {-8, 0});

    m_useMenu = CCMenu::create();
    m_useMenu->setID("use-menu");
    m_useMenu->setContentSize({90, size.height});
    m_useMenu->setAnchorPoint({1.f, 0.5f});
    this->addChildAtPosition(m_useMenu, Anchor::Right, {-8, 0});

    auto useSpr = ButtonSprite::create("Use", 66, 0, 0.6f, false, "bigFont.fnt", "GJ_button_01.png", 30);
    useSpr->setScale(.95);
    auto useBtn = CCMenuItemSpriteExtra::create(useSpr, this, menu_selector(ServerNode::onSelect));
    useBtn->setID("use-btn");
    useSpr->setCascadeOpacityEnabled(true);
    useBtn->setEnabled(list->m_selectedServer != m_server.id);
    m_useMenu->addChildAtPosition(useBtn, Anchor::Right, {-useBtn->getContentWidth() / 2, 0});

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

    m_editMenu->addChild(editBtn);
    m_editMenu->addChild(upBtn);
    m_editMenu->addChild(deleteBtn);
    m_editMenu->addChild(downBtn);

    m_editMenu->updateLayout();

    updateSelected(m_listLayer->m_servers[m_listLayer->m_selectedServer]);

    return true;
};

ServerNode *ServerNode::create(GDPSTypes::Server& server, CCSize size, ServerListLayer *list, int index) {
    auto ret = new ServerNode(server);
    if (ret && ret->init(size, list, index)) {
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
    auto btn = static_cast<CCMenuItemSpriteExtra *>(m_useMenu->getChildByID("use-btn"));
    if (!btn) return;
    auto spr = btn->getChildByType<ButtonSprite *>(0);
    if (!spr) return;
    if (server == m_server) {
        spr->updateBGImage("GJ_button_02.png");
        spr->setString("In Use");
        btn->setEnabled(false);
    } else {
        spr->updateBGImage("GJ_button_01.png");
        spr->setString("Use");
        btn->setEnabled(true);
    }
    auto alo = static_cast<AnchorLayoutOptions*>(btn->getLayoutOptions());
    // For some reason the CCMenuItemSpriteExtra gets changed so i gotta fix it... we love robtop code
    btn->setContentSize(spr->getScaledContentSize());   
    spr->setPosition(btn->getContentSize() / 2);
    alo->setOffset({-btn->getContentWidth() / 2, 0});
    btn->updateLayout();
    m_useMenu->updateLayout();
}

void ServerNode::updateInfo() {
    m_server = GDPSMain::get()->m_servers[m_server.id];
    if (auto nameLab = static_cast<CCLabelBMFont*>(this->getChildByID("name"))) {
        nameLab->setString(m_server.name.c_str());
        nameLab->limitLabelWidth(this->m_obContentSize.width - 150, .6f, 0.f);
    }

    // auto motdArea = static_cast<MDTextArea *>(this->getChildByID("motd"));
    auto motdArea = static_cast<ColorLabel *>(this->getChildByID("motd"));
    if (!motdArea) {
        // motdArea = MDTextArea::create(m_server.motd, {205.f, getContentHeight() - 27.f});
        motdArea = ColorLabel::create(m_server.motd, {205.f, getContentHeight() - 30.f}, .75f, 70);
        motdArea->setID("motd");
	    // motdArea->getChildByType<CCScale9Sprite>(0)->setVisible(false);
        // motdArea->getScrollLayer()->setTouchEnabled(false);
        // motdArea->getScrollLayer()->setMouseEnabled(false);
        motdArea->setAnchorPoint({0.f, 0.f});
        this->addChildAtPosition(motdArea, Anchor::BottomLeft, {61.f, 4.5f});
    }
    motdArea->setText(m_server.motd.c_str());
    if (auto icon = this->getChildByID("icon")) {
        icon->removeFromParent();
    }

    CCSprite *icon;

    if (m_server.iconIsSprite) {
        icon = CCSprite::createWithSpriteFrameName(m_server.icon.c_str());
        if (icon && !icon->getUserObject("geode.texture-loader/fallback")) {
            icon->setScale((getContentHeight() - 12.5f) / icon->getContentHeight());
        } else {
            icon = CCSpriteGrayscale::createWithSpriteFrameName("gdlogo.png"_spr);
            icon->setScale((getContentHeight() - 12.5f) / icon->getContentHeight());
        }
    } else if (m_server.icon.empty()) {
        icon = CCSpriteGrayscale::createWithSpriteFrameName("gdlogo.png"_spr);
        icon->setScale((getContentHeight() - 12.5f) / icon->getContentHeight());
    } else {
        auto ls = LazySprite::create({m_obContentSize.height - 12.5f, m_obContentSize.height - 12.5f}, true);
        ls->setAutoResize(true);
        ls->setScale(1.f);
        ls->setLoadCallback([this, &icon, ls](Result<> status) {
            if (status.isErr()) {
                ls->removeFromParent();
                icon = CCSpriteGrayscale::createWithSpriteFrameName("gdlogo.png"_spr);
                icon->setScale((getContentHeight() - 12.5f) / icon->getContentHeight());
                icon->setID("icon");
                this->addChildAtPosition(icon, Anchor::Left, {m_obContentSize.height / 2 + 2.5f, 0});
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
    createQuickPopup("Delete Server", fmt::format("Are you sure you want to delete {}? This will delete your save data for the server.", m_server.name), "No", "Yes", [this](auto, bool second) {
        if (second) {
            auto main = GDPSMain::get();
            main->m_shouldSaveGameData = false;
            auto serverPath = geode::dirs::getSaveDir() / "gdpses" / m_server.saveDir;
            auto gdpsesDir = geode::dirs::getSaveDir() / "gdpses";
            std::error_code err;
            if (std::filesystem::exists(serverPath, err)) {
                if (err) {
                    log::warn("Failed to check existence of {}: {}", serverPath, err.message());
                    MDPopup::create("Error", fmt::format("Failed to check save data for {}: {}", m_server.name, err.message()), "OK")->show();
                } else {
                    std::filesystem::path canonicalServerPath;
                    canonicalServerPath.clear();
                    canonicalServerPath = std::filesystem::canonical(serverPath, err);
                    if (err) {
                        log::warn("Failed to get canonical path for {}: {}", serverPath, err.message());
                        MDPopup::create("Error", fmt::format("Failed to resolve save data path for {}: {}", m_server.name, err.message()), "OK")->show();
                    } else if (canonicalServerPath.string().starts_with(gdpsesDir.string()) && serverPath != gdpsesDir) {
                        log::debug("Deleting {}", serverPath);
                        std::filesystem::remove_all(serverPath, err);
                        if (err) {
                            log::warn("Failed to delete server path {}: {}", serverPath, err.message());
                            MDPopup::create("Error", fmt::format("Failed to delete save data for {}: {}", m_server.name, err.message()), "OK")->show();
                        }
                    } else {
                        log::warn("Attempted to delete a path outside or equal to the gdpses directory: {}", serverPath);
                        MDPopup::create("Did not delete save", fmt::format("To prevent unintentional extra data loss, your save was not deleted - only saves within {} will be deleted. If you want to delete this data, do it manually.", gdpsesDir), "OK")->show();
                    }
                }
            } else if (err) {
                log::warn("Failed to check existence of {}: {}", serverPath, err.message());
                MDPopup::create("Error", fmt::format("Failed to check save data for {}: {}", m_server.name, err.message()), "OK")->show();
            }
            if (m_listLayer->m_selectedServer == m_server.id) {
                m_listLayer->m_selectedServer = -2;
                Mod::get()->setSavedValue("current", -2);
                GDPSMain::get()->m_shouldSaveGameData = false;
            }
            main->m_servers.erase(m_server.id);
            main->save();
            m_listLayer->updateList();
        }
    });
}

void ServerNode::setEditing(bool editing) {
    m_editing = editing;
    m_useMenu->setVisible(!m_editing);
    m_editMenu->setVisible(m_editing && !m_locked);
    auto order = Mod::get()->getSavedValue<std::vector<int>>("server-order", {});
    auto it = std::find(order.begin(), order.end(), m_server.id);
    if (auto btn = m_editMenu->getChildByID("up-btn")) {
        bool isSecond = it != order.end() && std::next(order.begin()) == it;
        btn->setVisible(!isSecond);
    }
    if (auto btn = m_editMenu->getChildByID("down-btn")) {
        bool isLast = (it != order.end() && std::prev(order.end()) == it);
        btn->setVisible(!isLast);
    }
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
