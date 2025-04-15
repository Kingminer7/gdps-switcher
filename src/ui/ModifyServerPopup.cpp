#include "ModifyServerPopup.hpp"
#include "utils/GDPSMain.hpp"
#include "Types.hpp"

bool ModifyServerPopup::setup(GDPSTypes::Server server, ServerListLayer * layer) {
    this->m_listLayer = layer;
    this->m_isNew = server.empty();
    this->m_server = server;
    this->m_orig = server;
    this->setTitle(this->m_isNew ? "Add Server" : "Edit Server");
    this->setID("ModifyServerPopup"_spr);
    m_buttonMenu->setID("button-menu");
    m_closeBtn->setID("close-button");
    m_mainLayer->setID("main-layer");
    m_title->setID("title");

    m_nameInput = TextInput::create(m_mainLayer->getContentWidth() - 40, "Server Name");
    m_nameInput->setID("name-input");
    m_nameInput->setString(server.name);
    m_nameInput->setCallback([this](const std::string &text) {
        m_server.name = text;
    });
    m_nameInput->setFilter("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;= ");
    m_mainLayer->addChildAtPosition(m_nameInput, Anchor::Center, {0.f, 25.f});

    m_urlInput = TextInput::create(m_mainLayer->getContentWidth() - 40, "Server URL");
    m_urlInput->setID("url-input");
    m_urlInput->setString(server.url);
    m_urlInput->setFilter("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=");
    m_urlInput->setCallback([this](const std::string &text) {
        m_server.url = text;
    });
    m_mainLayer->addChildAtPosition(m_urlInput, Anchor::Center, {0.f, -15.f});

    auto saveSpr = ButtonSprite::create("Save","bigFont.fnt","geode.loader/GE_button_05.png", .75f);
    saveSpr->setScale(.8f);
    auto saveBtn = CCMenuItemSpriteExtra::create(
        saveSpr,
        this, menu_selector(ModifyServerPopup::onSave)
    );
    saveBtn->setID("save-button");
    m_buttonMenu->addChildAtPosition(saveBtn, Anchor::Bottom, {0, saveBtn->getContentHeight() - 2.f});

    return true;
};

ModifyServerPopup *ModifyServerPopup::create(GDPSTypes::Server server, ServerListLayer * layer) {
    auto ret = new ModifyServerPopup();
    if (ret->initAnchored(240.f, 160.f, server, layer, "geode.loader/GE_square03.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void ModifyServerPopup::onClose(cocos2d::CCObject *sender) {
    if (m_orig == m_server) return Popup::onClose(sender);
    if (m_isNew) {
        createQuickPopup("Are you sure?", "Are you sure you want to discard this server?", "No", "Yes", [this, sender](auto, bool btn2) {
            if (btn2) {
                Popup::onClose(sender);
            }
        });
    } else {
        createQuickPopup("Are you sure?", "Are you sure you want to discard your changes?", "No", "Yes", [this, sender](auto, bool btn2) {
            if (btn2) {
                Popup::onClose(sender);
            }
        });
    }
}

void ModifyServerPopup::onSave(cocos2d::CCObject *sender) {
    if (m_server.url == "" || m_server.name == "") return;
    auto gdpsMain = GDPSMain::get();
    if (m_isNew) {
        if (std::find(gdpsMain->m_servers.begin(), gdpsMain->m_servers.end(), m_server) != gdpsMain->m_servers.end()) {
            return;
        }
        gdpsMain->m_servers.push_back(m_server);
    } else {
        auto res = std::find(gdpsMain->m_servers.begin(), gdpsMain->m_servers.end(), m_server);
        if (res == gdpsMain->m_servers.end()) {
            return;
        }
        GDPSTypes::Server& s = *res;
        s.name = m_nameInput->getString();
        s.url = m_urlInput->getString();
    }
    m_listLayer->updateList();
    Mod::get()->setSavedValue<std::vector<GDPSTypes::Server>>("saved-servers", GDPSMain::get()->m_servers);
    Popup::onClose(sender);
}