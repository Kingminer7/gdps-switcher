#include "ModifyServerPopup.hpp"
#include "utils/ServerInfoManager.hpp"
#include "Geode/ui/BasedButtonSprite.hpp"
#include "utils/GDPSMain.hpp"
#include "Types.hpp"
#include <regex>
#include <km7dev.server_api/include/ServerAPIEvents.hpp>
#include "../hooks/GManager.hpp"

bool ModifyServerPopup::setup(GDPSTypes::Server server, ServerListLayer * layer) {
    this->m_listLayer = layer;
    this->m_isNew = server.empty();
    this->m_server = server;
    this->m_orig = server;
    this->setTitle(this->m_isNew ? "Add Server" : "Edit Server");
    this->setID("ModifyServerPopup"_spr);
    m_closeBtn->removeFromParent();
    m_closeBtn = CCMenuItemSpriteExtra::create(
        CircleButtonSprite::createWithSpriteFrameName("geode.loader/close.png", .9f, CircleBaseColor::DarkPurple, CircleBaseSize::Small),
        this, menu_selector(ModifyServerPopup::onClose)
    );
    m_buttonMenu->addChildAtPosition(m_closeBtn, Anchor::TopLeft, {3.f, -3.f});
    m_buttonMenu->setID("button-menu");
    m_closeBtn->setID("close-button");
    m_mainLayer->setID("main-layer");
    m_title->setID("title");

    auto nameTitle = CCLabelBMFont::create("Server Name", "bigFont.fnt");
    nameTitle->setID("name-title");
    nameTitle->setScale(.4f);
    m_mainLayer->addChildAtPosition(nameTitle, Anchor::Center, {0.f, 70.f});

    m_nameInput = TextInput::create(m_mainLayer->getContentWidth() - 40, "Server Name");
    m_nameInput->setID("name-input");
    m_nameInput->setString(server.name);
    m_nameInput->setCallback([this](const std::string &text) {
        m_server.name = text;
        checkValidity();
    });
    m_nameInput->setFilter("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=% ");
    m_mainLayer->addChildAtPosition(m_nameInput, Anchor::Center, {0.f, 48.f});

    auto urlTitle = CCLabelBMFont::create("Server URL", "bigFont.fnt");
    urlTitle->setID("url-title");
    urlTitle->setScale(.4f);
    m_mainLayer->addChildAtPosition(urlTitle, Anchor::Center, {0.f, 25.f});

    m_urlInput = TextInput::create(m_mainLayer->getContentWidth() - 40, "Server URL");
    m_urlInput->setID("url-input");
    m_urlInput->setString(server.url);
    m_urlInput->setFilter("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%");
    m_urlInput->setCallback( [this](const std::string &text) {
        m_server.url = text;
        checkValidity();
    });
    m_mainLayer->addChildAtPosition(m_urlInput, Anchor::Center, {0.f, 3.f});

    auto saveTitle = CCLabelBMFont::create("Save Name (optional)", "bigFont.fnt");
    saveTitle->setID("save-title");
    saveTitle->setScale(.4f);
    m_mainLayer->addChildAtPosition(saveTitle, Anchor::Center, {0.f, -20.f});

    m_saveInput = TextInput::create(m_mainLayer->getContentWidth() - 40, "Save Name (optional)");
    m_saveInput->setID("save-input");
    m_saveInput->setString(server.saveDir == fmt::format("{}", server.id) ? "" : server.saveDir);
    m_saveInput->setFilter("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._/");
    m_saveInput->setCallback([this](const std::string &text) {
        m_server.saveDir = text;
    });
    m_mainLayer->addChildAtPosition(m_saveInput, Anchor::Center, {0.f, -42.f});

    auto saveSpr = ButtonSprite::create("Save","bigFont.fnt","geode.loader/GE_button_05.png", .75f);
    saveSpr->setScale(.8f);
    m_saveBtn = CCMenuItemSpriteExtra::create(
        saveSpr,
        this, menu_selector(ModifyServerPopup::onSave)
    );
    m_saveBtn->setID("save-button");
    m_buttonMenu->addChildAtPosition(m_saveBtn, Anchor::Bottom, {0, m_saveBtn->getContentHeight() - 2.f});

    checkValidity();

    return true;
};

ModifyServerPopup *ModifyServerPopup::create(GDPSTypes::Server server, ServerListLayer * layer) {
    auto ret = new ModifyServerPopup();
    if (ret->initAnchored(300.f, 220.f, server, layer, "geode.loader/GE_square03.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void ModifyServerPopup::checkValidity() {
    // Regex is scary!
    static std::basic_regex urlRegex = std::regex("(http|https):\\/\\/([\\w_-]+(?:(?:\\.[\\w_-]+)+))([\\w.,@?^=%&:\\/~+#-]*[\\w@?^=%&\\/~+#-])");
    bool valid = true;
    
    if (!m_server.url.empty()) {
        if (!std::regex_match(m_server.url, urlRegex)){
            m_urlInput->getInputNode()->setLabelNormalColor({255,100,100});
            valid = false;
        } else {
            m_urlInput->getInputNode()->setLabelNormalColor({255,255,255});
        }
    }
    if (m_server.name.empty() || m_server.url.empty()) {
        valid = false;
    }
    if (valid) {
        m_saveBtn->setEnabled(true);
        auto spr = static_cast<CCSprite*>(m_saveBtn->getNormalImage());
        spr->setCascadeColorEnabled(true);
        spr->setCascadeOpacityEnabled(true);
        spr->setColor({255,255,255});
        spr->setOpacity(255);
    } else {
        m_saveBtn->setEnabled(false);
        auto spr = static_cast<CCSprite*>(m_saveBtn->getNormalImage());
        spr->setCascadeColorEnabled(true);
        spr->setCascadeOpacityEnabled(true);
        spr->setColor(ccGRAY);
        spr->setOpacity(155);
    }
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

static std::string urlToFilenameSafe(std::string_view url) {
    std::string ret = "";
    ret.reserve(url.size());
    for (char c : url) {
        if (std::isalnum(c) || c == '.' || c == '-' || c == '_') {
            ret += c;
        } else {
            ret += '_';
        }
    }
    return ret;
}

void ModifyServerPopup::onSave(cocos2d::CCObject *sender) {
    if (m_server.url == "" || m_server.name == "") return;
    
    auto gdpsMain = GDPSMain::get();
    if (m_isNew) {
        if (gdpsMain->m_servers.contains(m_server.id)) {
            return;
        }
        gdpsMain->m_servers[m_server.id] = m_server;
        if (m_saveInput->getString().empty()) {
            gdpsMain->m_servers[m_server.id].saveDir = fmt::format("{}", m_server.id);
        }
        ServerInfoManager::get()->fetch(m_server);
    } else {
        auto &server = gdpsMain->m_servers[m_server.id];
        if (!gdpsMain->m_servers.contains(server.id)) {
            return;
        }
        auto newSaveDir = m_saveInput->getString().empty() ? fmt::format("{}", m_server.id) : m_saveInput->getString();
        if (newSaveDir != server.saveDir) {
            auto path = geode::dirs::getSaveDir() / "gdpses" / newSaveDir;
            std::error_code err;
            bool exists = std::filesystem::exists(path, err);
            if (err) {
                log::error("Failed to check if save directory exists at {}: {}", path.string(), err.message());
                MDPopup::create("Error", fmt::format("Failed to check if save directory exists at {}: {}", path.string(), err.message()), "OK")->show();
                return;
            }
            if (exists) {
                auto gdpsesDir = geode::dirs::getSaveDir() / "gdpses";
                if (!std::filesystem::canonical(path).string().starts_with(gdpsesDir.string()) || std::filesystem::equivalent(path, gdpsesDir)) {
                    log::warn("{} already exists and is not part of the gdpses subdirectory or is the gdpses directory itself - will not delete.", path.string());
                    MDPopup::create("Hold up!", fmt::format("The save directory you chose already exists! GDPS Switcher will not automatically delete anything outside of `{}` or the directory itself to prevent extra data loss.", gdpsesDir.string()), "OK")->show();
                    return;
                }
                geode::createQuickPopup("Hold up!", "The save directory you chose already exists! Do you want to overwrite it?", "No", "Yes", [this, path, newSaveDir, sender](auto, bool yes) {
                    if (yes) {
                        auto gdpsMain = GDPSMain::get();
                        auto &server = gdpsMain->m_servers[m_server.id];
                        std::error_code err;
                        std::filesystem::remove_all(path, err);
                        if (err) {
                            log::warn("Failed to delete existing save directory at {}: {}", path.string(), err.message());
                            MDPopup::create("Error", fmt::format("Failed to delete existing save directory at {}: {}", path.string(), err.message()), "OK")->show();
                            return;
                        }
                        bool exists = std::filesystem::exists(geode::dirs::getSaveDir() / "gdpses" / server.saveDir, err);
                        if (err) {
                            log::warn("Failed to check if existing save directory exists at {}: {}", (geode::dirs::getSaveDir() / "gdpses" / server.saveDir).string(), err.message());
                            MDPopup::create("Error", fmt::format("Failed to check if existing save directory exists at {}: {}", (geode::dirs::getSaveDir() / "gdpses" / server.saveDir).string(), err.message()), "OK")->show();
                            return;
                        }
                        if (exists) {
                            std::filesystem::rename(geode::dirs::getSaveDir() / "gdpses" / server.saveDir, path, err);
                            if (err) {
                                log::warn("Failed to rename save directory from {} to {}: {}", (geode::dirs::getSaveDir() / "gdpses" / server.saveDir).string(), path.string(), err.message());
                                MDPopup::create("Error", fmt::format("Failed to rename save directory from {} to {}: {}", (geode::dirs::getSaveDir() / "gdpses" / server.saveDir).string(), path.string(), err.message()), "OK")->show();
                                return;
                            }
                        }
                        server.saveDir = newSaveDir;
                        server.name = m_nameInput->getString();
                        server.url = m_urlInput->getString();
                        if (gdpsMain->m_currentServer == server.id) {
                            ServerAPIEvents::updateServer(GDPSMain::get()->m_serverApiId, server.url);
                            GSGManager::updateFileNames();
                        }
                        ServerInfoManager::get()->fetch(server);
                        m_listLayer->updateList();
                        GDPSMain::get()->save();
                        Popup::onClose(sender);
                    }
                });
                return;
            }
            std::string oldSaveDir = server.saveDir.empty() ? fmt::format("{}", server.id) : server.saveDir;
            exists = std::filesystem::exists(geode::dirs::getSaveDir() / "gdpses" / oldSaveDir, err);
            if (err) {
                log::error("Failed to delete existing save directory at {}: {}", path.string(), err.message());
                MDPopup::create("Error", fmt::format("Failed to delete existing save directory at {}: {}", path.string(), err.message()), "OK")->show();
                return;
            }
            if (exists) {
                std::filesystem::rename(geode::dirs::getSaveDir() / "gdpses" / server.saveDir, path, err);
                if (err) {
                    log::error("Failed to rename save directory from {} to {}: {}", (geode::dirs::getSaveDir() / "gdpses" / server.saveDir).string(), path.string(), err.message());
                    MDPopup::create("Error", fmt::format("Failed to rename save directory from {} to {}: {}", (geode::dirs::getSaveDir() / "gdpses" / server.saveDir).string(), path.string(), err.message()), "OK")->show();
                    return;
                }
            }
            server.saveDir = newSaveDir;
            if (gdpsMain->m_currentServer == server.id) {
                GSGManager::updateFileNames();
            }
        }
        server.name = m_nameInput->getString();
        server.url = m_urlInput->getString();
        if (gdpsMain->m_currentServer == server.id) {
            ServerAPIEvents::updateServer(GDPSMain::get()->m_serverApiId, server.url);
        }
        ServerInfoManager::get()->fetch(server);
    }
    m_listLayer->updateList();
    GDPSMain::get()->save();
    Popup::onClose(sender);
}
