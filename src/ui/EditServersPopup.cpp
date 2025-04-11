#include "EditServersPopup.hpp"
#include "ModifyServerPopup.hpp"
#include "utils/GDPSMain.hpp"

bool ServerEditNode::init(GDPSTypes::Server server, EditServersPopup *popup) {
    if (!CCNode::init()) return false;
    this->m_popup = popup;
    this->setAnchorPoint({0.5, 0.5});
    this->setContentSize({290.f, 30.f});

    auto bg = cocos2d::extension::CCScale9Sprite::create("square02b_small.png", {0, 0, 40, 40});
    bg->setContentSize({290.f, 30.f});
    bg->setOpacity(30);
    bg->setColor({94, 94, 255});
    this->addChildAtPosition(bg, Anchor::Center);

    m_name = CCLabelBMFont::create(server.name.c_str(), "bigFont.fnt");
    m_name->setAnchorPoint({0,0.5});
    m_name->limitLabelWidth(200, .7f, .1f);
    this->addChildAtPosition(m_name, Anchor::Left, {3, 0});

    m_menu = CCMenu::create();
    m_menu->setAnchorPoint({1, 0.5});
    m_menu->setContentSize({85, 25});
    m_menu->ignoreAnchorPointForPosition(false);
    m_menu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::End)->setAxisReverse(true)->setGap(2));
    this->addChildAtPosition(m_menu, Anchor::Right, {-3, 0});

    auto editSpr = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName("edit.png"_spr), EditorBaseColor::Gray);
    editSpr->setScale(.75f);
    auto editBtn = CCMenuItemSpriteExtra::create(
        editSpr,
        this,
        menu_selector(ServerEditNode::onEdit)
    );

    auto deleteSpr = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"), EditorBaseColor::Gray);
    deleteSpr->setScale(.75f);
    if (auto x = deleteSpr->getChildByType<CCSprite *>(0)) {
        x->setScale(x->getScale() * .9);
    }
    auto deleteBtn = CCMenuItemSpriteExtra::create(
        deleteSpr,
        this,
        menu_selector(ServerEditNode::onDelete)
    );

    m_menu->addChild(deleteBtn);
    m_menu->addChild(editBtn);

    m_menu->updateLayout();

    m_server = server;

    return true;
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
    ModifyServerPopup::create(m_server, m_popup->m_listLayer)->show();
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
    m_scroll->m_contentLayer->removeAllChildren();
    auto servers = GDPSMain::get()->m_servers;
    m_scroll->m_contentLayer->setContentHeight(std::max(m_scroll->getContentHeight(), servers.size() * 33.f - 3));
    auto i = 0;
    for (auto server : servers) {
        i++;
        auto node = ServerEditNode::create(server, this);

        m_scroll->m_contentLayer->addChildAtPosition(node, Anchor::Top, {0, -i * 33.f + 18.f});
    }

    m_scroll->scrollToTop();
}

bool EditServersPopup::setup(ServerListLayer *layer) {
    this->m_listLayer = layer;

    auto scrollBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    scrollBg->setColor({0, 0, 0});
    scrollBg->setOpacity(90);
    scrollBg->setContentSize({304, 204});
    scrollBg->ignoreAnchorPointForPosition(false);
    scrollBg->setID("server-scroll-bg");
    m_mainLayer->addChildAtPosition(scrollBg, geode::Anchor::Center);
    
    m_scroll = geode::ScrollLayer::create({290, 200});
    m_scroll->setID("server-scroll");
    m_scroll->ignoreAnchorPointForPosition(false);
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
    if (ret->initAnchored(360.f, 260.f, layer, "geode.loader/GE_square01.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}