#include "BonusSettingsPopup.hpp"
#include "ui/ServerListLayer.hpp"

using namespace geode::prelude;

#define NodeSize ccp(360, 40)

#define makeButton(var, id, text, background, callback) \
    CCMenu* var = CCMenu::create(); \
    var->setContentSize(NodeSize); \
    var->setAnchorPoint({.5f, .5f}); \
    var->setID(id); \
    var->ignoreAnchorPointForPosition(false); \
     \
    { \
        CCLayerColor* bg = CCLayerColor::create(odd ? ccColor4B{0, 0, 0, 60} : ccColor4B{0, 0, 0, 30}, NodeSize.x, NodeSize.y); \
        bg->setAnchorPoint({0.5f, 0.5f}); \
        bg->ignoreAnchorPointForPosition(false); \
        var->setZOrder(-1); \
        var->addChildAtPosition(bg, Anchor::Center); \
        ButtonSprite* spr = ButtonSprite::create(text, 15 * strlen(text), 0, 0.7f, false, "bigFont.fnt", background, 30); \
        spr->setScale(.9f); \
        CCMenuItemSpriteExtra* btn = CCMenuItemExt::createSpriteExtra(spr, callback); \
        btn->setID("button"); \
        var->addChildAtPosition(btn, Anchor::Center); \
    } \
    m_scroll->m_contentLayer->addChild(var); \
    m_options.push_back(var); \
    odd = !odd;

#define makeToggle(var, text, id, value, default, callback) \
    CCMenu* var = CCMenu::create(); \
    var->setContentSize(NodeSize); \
    var->setAnchorPoint({.5f, .5f}); \
    var->setID(id); \
    var->ignoreAnchorPointForPosition(false); \
     \
    { \
        CCLayerColor* bg = CCLayerColor::create(odd ? ccColor4B{0, 0, 0, 60} : ccColor4B{0, 0, 0, 30}, NodeSize.x, NodeSize.y); \
        bg->setAnchorPoint({0.5f, 0.5f}); \
        bg->ignoreAnchorPointForPosition(false); \
        var->setZOrder(-1); \
        var->addChildAtPosition(bg, Anchor::Center); \
        CCMenuItemToggler* btn = CCMenuItemExt::createTogglerWithStandardSprites(.7f, callback); \
        btn->toggle(Mod::get()->getSavedValue<bool>(value, default)); \
        var->addChildAtPosition(btn, Anchor::Right, {-6 - btn->getScaledContentWidth() / 2, 0}); \
        CCLabelBMFont* txt = CCLabelBMFont::create(text, "bigFont.fnt"); \
        txt->limitLabelWidth(NodeSize.x - (btn->getScaledContentWidth() + 18), .7f, 0.f); \
        txt->setAnchorPoint({0.f, 0.5f}); \
        var->addChildAtPosition(txt, Anchor::Left, {6, 0}); \
    } \
    m_scroll->m_contentLayer->addChild(var); \
    m_options.push_back(var); \
    odd = !odd;

bool BonusSettingsPopup::setup() {
    m_scroll = ScrollLayer::create({360, 240});
    m_scroll->ignoreAnchorPointForPosition(false);
    m_scroll->setAnchorPoint({.5f, .5f});
    m_scroll->m_contentLayer->setLayout(
        AxisLayout::create(Axis::Column)
        ->setAutoScale(false)
        ->setAxisReverse(true)
        ->setAxisAlignment(AxisAlignment::End)
        ->setGap(0.f)
    );
    m_mainLayer->addChildAtPosition(m_scroll, Anchor::Center, {0.f, 0.f});
    auto lb = ListBorders::create();
    lb->setContentSize(m_scroll->getContentSize());
    m_mainLayer->addChildAtPosition(lb, Anchor::Center, {0.f, 0.f});

    bool odd = true;
    makeToggle(nintendoSwitcher, "Nintendo Switcher (2025 april fools)", "nintendo-switcher-toggle", "ss-nintendo-switcher", false, [this](CCMenuItemToggler *toggler){
        Mod::get()->setSavedValue("ss-nintendo-switcher", !toggler->m_toggled);
        m_reloadLayer = true;
    })
    makeButton(hideBtn, "hide-button", "Deactivate Menu", "GJ_button_01.png", [this](auto){ 
        geode::createQuickPopup("Are you sure?", "Are you sure you want to disable the secret settings?", "Cancel", "Yes", [this](FLAlertLayer* popup, bool second){
            if (second) {
                onClose(popup);
                Mod::get()->setSavedValue("secret-settings", false);
                if (auto sll = CCScene::get()->getChildByType<ServerListLayer>(0)) {
                    auto menu = sll->getChildByID("edit-menu");
                    if (!menu) return;
                    if (auto btn = sll->getChildByID("edit-menu")->getChildByID("settings-button")) {
                        btn->removeFromParent();
                    }
                    menu->updateLayout();
                    sll->m_konamiPos = 0;
                }
            }
        });
    })

    m_scroll->m_contentLayer->updateLayout();
    float y = 0;
    for (auto node : m_options) {
        y += node->getScaledContentHeight();
    }
    m_scroll->m_contentLayer->setContentHeight(std::max(m_scroll->getContentHeight(), y));
    m_scroll->scrollToTop();
    return true;
}

BonusSettingsPopup *BonusSettingsPopup::create() {
    auto ret = new BonusSettingsPopup();
    if (ret->initAnchored(400.f, 280.f)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void BonusSettingsPopup::onClose(CCObject *sender) {
    if (m_reloadLayer) {
        auto transition = CCTransitionFade::create(0.5f, ServerListLayer::scene());
	    CCDirector::get()->replaceScene(transition);
    } else {
        Popup::onClose(sender);
    }
    
}