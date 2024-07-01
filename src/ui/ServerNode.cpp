#include "ServerNode.hpp"
#include "ServerPopup.hpp"

ServerNode *ServerNode::create(ServerSwitchLayer *layer, ServerSwitchLayer::ServerEntry entry, CCSize size, bool selected)
{
    auto ret = new ServerNode;
    if (ret && ret->init(layer, entry, size, selected))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool ServerNode::init(ServerSwitchLayer *layer, ServerSwitchLayer::ServerEntry entry, CCSize size, bool selected)
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
    label->limitLabelWidth(size.width - 124, 0.8f, 0.1f);
    this->addChild(label);

    auto menu = CCMenu::create();
    menu->setPosition(0, 0);
    this->addChild(menu);

    editBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("editBtn.png"_spr),
        this,
        menu_selector(ServerNode::editServer));
    editBtn->setID("server-node-edit");
    editBtn->setPosition({size.width - 60, size.height / 2});
    editBtn->setContentSize({31, 31});
    as<CCSprite *>(editBtn->getChildren()->objectAtIndex(0))->setPosition({15.5, 15.5});
    as<CCSprite *>(editBtn->getChildren()->objectAtIndex(0))->setScale(0.775f);
    menu->addChild(editBtn);

    delBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("deleteBtn.png"_spr),
        this,
        menu_selector(ServerNode::deleteServer));
    delBtn->setID("server-node-delete");
    delBtn->setPosition({size.width - 95, size.height / 2});
    delBtn->setContentSize({31, 31});
    as<CCSprite *>(delBtn->getChildren()->objectAtIndex(0))->setPosition({15.5, 15.5});
    as<CCSprite *>(delBtn->getChildren()->objectAtIndex(0))->setScale(0.775f);
    menu->addChild(delBtn);

    if (selected)
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
    selectBtn->setPosition({size.width - 25, size.height / 2});
    menu->addChild(selectBtn);

    return true;
}

void ServerNode::preventDelete() {
    delBtn->setVisible(false);
    editBtn->setVisible(false);
}

void ServerNode::deleteServer(CCObject *)
{
    geode::createQuickPopup("Delete Server", "Are you sure you want to delete this server? This cannot be undone!", "No", "Yes", [this](auto, bool yes)
                            {
        if (yes)
        {
            auto servers = Mod::get()->getSavedValue<std::vector<ServerSwitchLayer::ServerEntry>>("saved-servers");
            servers.erase(std::remove_if(servers.begin(), servers.end(), [this](auto const &entry) {
                return entry.url == server.url && entry.name == server.name;
            }));
            Mod::get()->setSavedValue("saved-servers", servers);
            m_layer->update(servers, false);
        } });
}

void ServerNode::editServer(CCObject *)
{
    auto popup = ServerPopup::create(m_layer, this);
    popup->show();
}

void ServerNode::selectServer(CCObject *)
{
    m_layer->selectServer(this);
}

ServerSwitchLayer::ServerEntry ServerNode::getServer()
{
    return server;
}