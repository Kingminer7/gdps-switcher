#include "EditServersPopup.hpp"
#include "ModifyServerPopup.hpp"
#include "utils/GDPSMain.hpp"
#include "extra/Drag.hpp"

bool ServerEditNode::init(GDPSTypes::Server server, EditServersPopup *popup) {
    if (!DragNode::init(static_cast<DragLayer*>(popup->m_scroll->m_contentLayer))) return false;
    this->m_popup = popup;
    this->setAnchorPoint({0.5, 0.5});
    this->setContentSize({290.f, 30.f});
    this->setID("server-edit-node");

    auto bg = cocos2d::extension::CCScale9Sprite::create("square.png", {0, 0, 40, 40});
    bg->setContentSize({290.f, 30.f});
    bg->setOpacity(60);
    bg->setColor({0, 0, 0});
    bg->setID("background");
    this->addChildAtPosition(bg, Anchor::Center);

    m_name = CCLabelBMFont::create(server.name.c_str(), "bigFont.fnt");
    m_name->setAnchorPoint({0, 0.5});
    m_name->limitLabelWidth(200, .7f, .1f);
    m_name->setID("server-name-label");
    this->addChildAtPosition(m_name, Anchor::Left, {3, 0});

    m_menu = CCMenu::create();
    m_menu->setAnchorPoint({1, 0.5});
    m_menu->setContentSize({85, 25});
    m_menu->ignoreAnchorPointForPosition(false);
    m_menu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::End)->setAxisReverse(true)->setGap(2));
    m_menu->setID("server-menu");
    this->addChildAtPosition(m_menu, Anchor::Right, {-3, 0});

    auto editSpr = CCSprite::create("geode.loader/GE_button_05.png");
    editSpr->setScale(.575f);
    auto editIcon = CCSprite::createWithSpriteFrameName("edit.png"_spr);
    editIcon->setScale(editSpr->getContentHeight() / editIcon->getContentHeight() * .8f);
    editSpr->addChildAtPosition(editIcon, Anchor::Center, {0.f, 0.f}, false);
    auto editBtn = CCMenuItemSpriteExtra::create(
        editSpr,
        this,
        menu_selector(ServerEditNode::onEdit)
    );
    editBtn->setID("edit-button");

    auto deleteSpr = CCSprite::create("geode.loader/GE_button_05.png");
    deleteSpr->setScale(.575f);
    auto deleteIcon = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
    deleteIcon->setScale(deleteSpr->getContentHeight() / deleteIcon->getContentHeight() * .8f);
    deleteSpr->addChildAtPosition(deleteIcon, Anchor::Center, {0.f, 0.f}, false);
    auto deleteBtn = CCMenuItemSpriteExtra::create(
        deleteSpr,
        this,
        menu_selector(ServerEditNode::onDelete)
    );
    deleteBtn->setID("delete-button");

    m_menu->addChild(deleteBtn);
    m_menu->addChild(editBtn);

    m_menu->updateLayout();

    m_server = server;

    return true;
}

GDPSTypes::Server ServerEditNode::getServer() {
    return m_server;
}

ServerEditNode *ServerEditNode::create(GDPSTypes::Server server, EditServersPopup *popup) {
    auto ret = new ServerEditNode();
    if (ret->init(server, popup)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void ServerEditNode::onEdit(CCObject *sender) {
    ModifyServerPopup::create(m_server, m_popup)->show();
}

void ServerEditNode::onDelete(CCObject *sender) {
    createQuickPopup("Delete Server", fmt::format("Are you sure you want to delete {}?", m_server.name), "No", "Yes", [this](auto, bool second) {
        if (second) {
            GDPSMain::get()->m_servers.erase(
                std::remove_if(
                    GDPSMain::get()->m_servers.begin(),
                    GDPSMain::get()->m_servers.end(),
                    [this](const GDPSTypes::Server& server) {
                        return server.name == m_server.name;
                    }
                ),
                GDPSMain::get()->m_servers.end()
            );
            m_popup->updateList();
        }
    });
}

void EditServersPopup::onExit() {
    m_listLayer->updateList();
    Popup::onExit();
}

void EditServersPopup::updateList() {
    if (auto dragLayer = static_cast<DragLayer*>(m_scroll->m_contentLayer)) {
        dragLayer->removeAllNodes();
        dragLayer->removeAllChildren();
    }
    auto servers = GDPSMain::get()->m_servers;
    m_scroll->m_contentLayer->setContentHeight(std::max(m_scroll->getContentHeight(), servers.size() * 33.f - 3));
    auto i = 0;
    for (auto server : servers) {
        i++;
        auto node = ServerEditNode::create(server, this);
        if (auto bg = static_cast<CCSprite *>(node->getChildByID("background"))) {
            bg->setOpacity(i % 2 == 0 ? 0 : 60);
        }
        node->setID(server.name);
        m_scroll->m_contentLayer->addChildAtPosition(node, Anchor::Top, {0, -i * 30.f + 15.f});
    }

    m_scroll->scrollToTop();
}

bool EditServersPopup::setup(ServerListLayer *layer) {
    this->m_listLayer = layer;

    auto scrollBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    scrollBg->setColor({0, 0, 0});
    scrollBg->setOpacity(40);
    scrollBg->setContentSize({304, 204});
    scrollBg->ignoreAnchorPointForPosition(false);
    scrollBg->setID("server-scroll-bg");
    m_mainLayer->addChildAtPosition(scrollBg, geode::Anchor::Center);
    
    m_scroll = geode::ScrollLayer::create({290, 200});
    m_scroll->setID("server-scroll");
    m_scroll->ignoreAnchorPointForPosition(false);
    m_scroll->m_contentLayer->removeFromParent();
    auto dragLayer = DragLayer::create(290, 200);
    dragLayer->onReorder([this](std::vector<DragNode *> nodes) {
        std::vector<GDPSTypes::Server> newOrder;
        for (auto node : nodes) {
            if (auto serverNode = dynamic_cast<ServerEditNode *>(node)) {
                newOrder.push_back(serverNode->getServer());
            }
        }
        GDPSMain::get()->m_servers = newOrder;
    });
    dragLayer->setID("content-layer");
    dragLayer->setAnchorPoint({ 0, 0 });
    m_scroll->m_contentLayer = dragLayer;
    m_scroll->addChild(dragLayer);
    
    auto scrollbar = geode::Scrollbar::create(m_scroll);
    scrollbar->setID("scrollbar");
    scrollbar->setScaleY(.95f);
    m_mainLayer->addChildAtPosition(scrollbar, geode::Anchor::Center, {146, 0.f});

    auto clip = cocos2d::CCClippingNode::create();
    clip->setID("server-list");
    clip->setZOrder(1);
    clip->setContentSize({290, 200});
    clip->setAnchorPoint({0.5, 0.5});
    clip->setAlphaThreshold(0.05f);
    clip->addChildAtPosition(m_scroll, geode::Anchor::Center);
    m_mainLayer->addChildAtPosition(clip, geode::Anchor::Center, {-5, 0});

    auto stencil = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    stencil->setContentSize({290, 200});
    stencil->setPosition({145, 100});
    clip->setStencil(stencil);

    updateList();

    return true;
};

EditServersPopup *EditServersPopup::create(ServerListLayer *layer) {
    auto ret = new EditServersPopup();
    if (ret->initAnchored(360.f, 260.f, layer, "geode.loader/GE_square03.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}