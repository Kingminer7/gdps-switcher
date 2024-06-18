#include "ServerNode.hpp"
#include "../utils/InstanceManager.hpp"

ServerNode *ServerNode::create(ServerSwitchLayer *layer, ServerSwitchLayer::ServerEntry entry, CCSize size)
{
    auto ret = new ServerNode;
    if (ret && ret->init(layer, entry, size))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool ServerNode::init(ServerSwitchLayer *layer, ServerSwitchLayer::ServerEntry entry, CCSize size)
{
    if (!CCNode::init())
        return false;
    this->setContentSize(size);
    m_layer = layer;
    server = entry;

    auto bg = CCScale9Sprite::create("GJ_square01.png", {0, 0, 80, 80});
    bg->setContentSize(size);
    bg->setPosition(size / 2);
    bg->setID("server-node-bg");
    this->addChild(bg);

    auto label = CCLabelBMFont::create(entry.name.c_str(), "bigFont.fnt");
    label->setScale(0.8f);
    label->setPosition(8, size.height / 2 + 2);
    label->setAnchorPoint({0, 0.5});
    this->addChild(label);

    auto menu = CCMenu::create();
    menu->setPosition(0, 0);
    this->addChild(menu);

    auto edit = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("editBtn.png"_spr),
        this,
        menu_selector(ServerNode::editServer));
    edit->setID("server-node-edit");
    edit->setPosition({355, size.height / 2});
    edit->setContentSize({31, 31});
    as<CCSprite*>(edit->getChildren()->objectAtIndex(0))->setPosition({15.5, 15.5});
    as<CCSprite*>(edit->getChildren()->objectAtIndex(0))->setScale(0.775f);
    menu->addChild(edit);

    auto del = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("deleteBtn.png"_spr),
        this,
        menu_selector(ServerNode::deleteServer));
    del->setID("server-node-delete");
    del->setPosition({320, size.height / 2});
    del->setContentSize({31, 31});
    as<CCSprite*>(del->getChildren()->objectAtIndex(0))->setPosition({15.5, 15.5});
    as<CCSprite*>(del->getChildren()->objectAtIndex(0))->setScale(0.775f);
    menu->addChild(del);

    if (InstanceManager::get()->getServer() == entry.url)
    {
        selectBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
            this,
            menu_selector(ServerNode::selectServer));
    }
    else
    {
        selectBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
            this,
            menu_selector(ServerNode::selectServer));
    }

    selectBtn->setID("server-node-select");
    selectBtn->setPosition({390, size.height / 2});
    menu->addChild(selectBtn);

    return true;
}

void ServerNode::deleteServer(CCObject *)
{
    auto servers = Mod::get()->getSavedValue<std::vector<ServerSwitchLayer::ServerEntry>>("saved-servers");
    servers.erase(std::remove_if(servers.begin(), servers.end(), [this](auto const &entry) {
        return entry.url == server.url && entry.name == server.name;
    }));
    Mod::get()->setSavedValue("saved-servers", servers);
    m_layer->update(servers, false);
}

void ServerNode::editServer(CCObject *)
{
}

void ServerNode::selectServer(CCObject *)
{
    Mod::get()->setSavedValue("server", server.url);
    auto servers = Mod::get()->getSavedValue<std::vector<ServerSwitchLayer::ServerEntry>>("saved-servers");
    m_layer->update(servers, false);
}
