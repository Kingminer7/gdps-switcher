#include "ModifyServerPopup.hpp"
#include "Geode/binding/ButtonSprite.hpp"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/loader/Mod.hpp"
#include "Geode/ui/Layout.hpp"
#include "Geode/ui/Popup.hpp"
#include "Geode/ui/TextInput.hpp"
#include "utils/GDPSMain.hpp"
#include "Types.hpp"
#include <algorithm>

bool ModifyServerPopup::setup(GDPSTypes::Server server, ServerListLayer * layer) {
    this->m_listLayer = layer;

    m_nameInput = geode::TextInput::create(m_mainLayer->getContentWidth() - 40, "Server Name");
    m_nameInput->setString(server.name);
    m_nameInput->setCallback([this](const std::string &text) {
        m_server.name = text;
    });
    m_nameInput->setFilter("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;= ");
    m_mainLayer->addChildAtPosition(m_nameInput, geode::Anchor::Center, {0.f, 25.f});

    m_urlInput = geode::TextInput::create(m_mainLayer->getContentWidth() - 40, "Server URL");
    m_urlInput->setString(server.url);
    m_urlInput->setFilter("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=");
    m_urlInput->setCallback([this](const std::string &text) {
        m_server.url = text;
    });
    m_mainLayer->addChildAtPosition(m_urlInput, geode::Anchor::Center, {0.f, -15.f});

    m_isNew = server.empty();

    auto saveSpr = ButtonSprite::create("Save","bigFont.fnt","geode.loader/GE_button_05.png", .75f);
    saveSpr->setScale(.8f);
    auto saveBtn = CCMenuItemSpriteExtra::create(
        saveSpr,
        this, menu_selector(ModifyServerPopup::onSave)
    );
    m_buttonMenu->addChildAtPosition(saveBtn, geode::Anchor::Bottom, {0, saveBtn->getContentHeight() - 2.f});

    m_server = server;

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
    if (m_isNew) {
        geode::createQuickPopup("Are you sure?", "Are you sure you want to discard this server?", "No", "Yes", [this, sender](auto, bool btn2) {
            if (btn2) {
                Popup::onClose(sender);
            }
        });
    } else {
        geode::createQuickPopup("Are you sure?", "Are you sure you want to discard your changes?", "No", "Yes", [this, sender](auto, bool btn2) {
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
        Server& s = *std::find(gdpsMain->m_servers.begin(), gdpsMain->m_servers.end(), m_server);
        if (s.empty()) {
            return;
        }
        s.name = m_nameInput->getString();
        s.url = m_urlInput->getString();
    }
    m_listLayer->updateList();
    geode::Mod::get()->setSavedValue<std::vector<GDPSTypes::Server>>("saved-servers", GDPSMain::get()->m_servers);
    Popup::onClose(sender);
}