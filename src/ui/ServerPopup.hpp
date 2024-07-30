using namespace geode::prelude;
#include "ServerSwitchLayer.hpp"

class ServerPopup : public Popup<ServerSwitchLayer *, ServerNode *>
{
private:
    TextInput *m_nameInput;
    TextInput *m_urlInput;
    ServerSwitchLayer *m_layer;
    ServerNode *m_node;

protected:
    bool setup(ServerSwitchLayer *layer, ServerNode *node) override
    {
        m_layer = layer;
        m_node = node;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        this->setTitle((node == nullptr) ? "Add Server" : "Edit Server");

        auto menu = CCMenu::create();
        menu->setID("data-menu");
        menu->setZOrder(10);
        m_mainLayer->addChildAtPosition(menu, Anchor::Center, ccp(0, 0));

        auto nameInpLab = CCLabelBMFont::create("Server Name", "bigFont.fnt");
        nameInpLab->setPosition(ccp(0, 5));
        nameInpLab->setScale(.35f);
        nameInpLab->setID("name-label");
        nameInpLab->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        menu->addChild(nameInpLab);

        m_nameInput = TextInput::create(280.f, "Enter Name", "bigFont.fnt");
        m_nameInput->getInputNode()->setLabelPlaceholderColor(ccColor3B{200, 200, 200});
        m_nameInput->setPosition(ccp(0, 28));
        m_nameInput->setID("name-input");
        m_nameInput->getInputNode()->setAllowedChars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;= ");
        m_nameInput->getInputNode()->setMaxLabelLength(24);
        if (node != nullptr)
            m_nameInput->setString(node->getServer().name);
        menu->addChild(m_nameInput);

        auto urlInpLab = CCLabelBMFont::create("Server URL", "bigFont.fnt");
        urlInpLab->setPosition(ccp(0, 50));
        urlInpLab->setScale(.35f);
        urlInpLab->setID("name-label");
        urlInpLab->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        menu->addChild(urlInpLab);

        m_urlInput = TextInput::create(280.f, "Enter URL", "bigFont.fnt");
        m_urlInput->getInputNode()->setLabelPlaceholderColor(ccColor3B{200, 200, 200});
        m_urlInput->setPosition(ccp(0, -17));
        m_urlInput->setID("url-input");
        m_urlInput->getInputNode()->setAllowedChars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=");
        if (node != nullptr)
            m_urlInput->setString(node->getServer().url);
        menu->addChild(m_urlInput);

        auto olderWarning = CCLabelBMFont::create(("Only servers that support " + Loader::get()->getGameVersion() + " will work").c_str(), "bigFont.fnt");
        // olderWarning->setColor({255, 127, 127});
        olderWarning->setPosition(ccp(0, -41));
        olderWarning->setScale(.35f);
        olderWarning->setID("version-warning");
        olderWarning->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        menu->addChild(olderWarning);

        auto saveBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Save", "goldFont.fnt", "GJ_button_01.png", .8f),
            this, menu_selector(ServerPopup::onSave));
        saveBtn->setPosition(ccp(0, -66));
        saveBtn->setID("save-button");
        menu->addChild(saveBtn);

        return true;
    }

    void onClose(cocos2d::CCObject *) override
    {
        if (m_node != nullptr)
        {
            if (m_nameInput->getString() == m_node->getServer().name && m_urlInput->getString() == m_node->getServer().url)
            {
                this->setKeypadEnabled(false);
                this->setTouchEnabled(false);
                this->removeFromParentAndCleanup(true);
                return;
            }
            else
            {
                geode::createQuickPopup("Cancel?", "Your changes will not be saved.", "No", "Yes", [this](auto, bool yes)
                                        {
            if (yes)
            {
                this->setKeypadEnabled(false);
                this->setTouchEnabled(false);
                this->removeFromParentAndCleanup(true);
            } });
            }
        }
        else
        {
            if (m_nameInput->getString().empty() && m_urlInput->getString().empty())
            {
                this->setKeypadEnabled(false);
                this->setTouchEnabled(false);
                this->removeFromParentAndCleanup(true);
                return;
            }
            else
            {
                geode::createQuickPopup("Cancel?", "Your entry will be discarded.", "No", "Yes", [this](auto, bool yes)
                                        {
            if (yes)
            {
                this->setKeypadEnabled(false);
                this->setTouchEnabled(false);
                this->removeFromParentAndCleanup(true);
            } });
            }
        }
    }

    void onSave(CCObject *)
    {
        auto name = m_nameInput->getString();
        auto url = m_urlInput->getString();
        if (name.empty() || std::all_of(name.begin(), name.end(), isspace))
        {
            FLAlertLayer::create("Error", "Name cannot be empty.", "Ok")->show();
            return;
        }
        if (url.empty())
        {
            FLAlertLayer::create("Error", "URL cannot be empty.", "Ok")->show();
            return;
        }
        auto servers = Mod::get()->getSavedValue<std::vector<ServerSwitchLayer::ServerEntry>>("saved-servers");
        if (m_node != nullptr)
        {
            auto index = std::distance(servers.begin(), std::find_if(servers.begin(), servers.end(), [this](auto &entry) {
                return entry.name == m_node->getServer().name;
            }));
            servers[index] = {name, url};
        }
        else
        {
            servers.push_back({name, url});
        }
        Mod::get()->setSavedValue("saved-servers", servers);
        m_layer->update(servers, false);
        this->setKeypadEnabled(false);
        this->setTouchEnabled(false);
        this->removeFromParentAndCleanup(true);
    }

public:
    static ServerPopup *create(ServerSwitchLayer *layer, ServerNode *node = nullptr)
    {
        auto ret = new ServerPopup();
        if (ret && ret->initAnchored(310.f, 180.f, layer, node))
        {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};
