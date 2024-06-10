#include "ServerSwitchLayer.hpp"
#include <Geode/ui/General.hpp>
#include <Geode/ui/Notification.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/MenuLayer.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/modify/CCScrollLayerExt.hpp>

static CCSize LAYER_SIZE = {230.f, 190.f};

gd::string latestValue;

bool ServerSwitchLayer::init()
{
    if (!CCLayer::init())
        return false;

    this->setID("ServerSwitchLayer");

    this->setKeypadEnabled(true);

    auto background = createLayerBG();
    background->setID("background");
    this->addChild(background);

    auto winSize = CCDirector::get()->getWinSize();

    auto title = CCLabelBMFont::create("Private Server Switcher", "goldFont.fnt");
    title->setPosition(winSize.width / 2, winSize.height - 20.f);
    title->setID("title-text");
    this->addChild(title);

    auto newUrl = Mod::get()->getSavedValue<std::string>("server");
    if (newUrl.empty())
    {
        newUrl = "http://www.boomlings.com/database";
        Mod::get()->setSavedValue("server", newUrl);
    }

    input = CCTextInputNode::create(winSize.width * .9 / .75, 60, "Server IP", "bigFont.fnt");
    input->setPosition(winSize.width / 2 * .7, winSize.height - 70.f);
    input->setAllowedChars("0123456789.abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ:/");
    input->setScale(.75);
    input->setZOrder(1);
    input->setString(newUrl);
    latestValue = input->getString();
    input->setID("server-input");
    this->addChild(input);

    auto bg = CCScale9Sprite::create("square02b_001.png");
    bg->setPosition(winSize.width / 2, winSize.height - 65.f);
    bg->setScale(.5);
    bg->setContentSize(CCSize(winSize.width * .9 * 2, 60));
    bg->setID("input-bg");
    bg->setColor(ccColor3B({0, 56, 141}));
    this->addChild(bg);

    auto menu = CCMenu::create();
    menu->setID("menu");
    menu->setZOrder(10);

    auto applyBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", .8f),
        this, menu_selector(ServerSwitchLayer::onApply));
    applyBtn->setPosition(0,  winSize.height - 260.f);
    applyBtn->setID("apply-button");
    menu->addChild(applyBtn);

    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this, menu_selector(ServerSwitchLayer::onGoBack));
    backBtn->setPosition(-winSize.width / 2 + 25.f, winSize.height / 2 - 25.f);
    backBtn->setID("back-button");
    menu->addChild(backBtn);
    this->addChild(menu);

    return true;
}

void ServerSwitchLayer::keyBackClicked()
{
    this->onGoBack(nullptr);
}

void ServerSwitchLayer::onGoBack(CCObject *)
{
    if (latestValue == input->getString())
    {
        CCDirector::get()->replaceScene(CCTransitionFade::create(0.5, MenuLayer::scene(false)));
    }
    else
    {
        geode::createQuickPopup(
            "Exit without saving?",      // title
            "You have unsaved changes.", // content
            "No", "Yes",                 // buttons
            [](auto, bool btn2)
            {
                if (btn2)
                {
                    CCDirector::get()->replaceScene(CCTransitionFade::create(0.5, MenuLayer::scene(false)));
                }
            });
    }
}

ServerSwitchLayer *ServerSwitchLayer::create()
{
    auto ret = new ServerSwitchLayer;
    if (ret->init())
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CCLayer *ServerSwitchLayer::scene()
{
    auto layer = ServerSwitchLayer::create();
    switchToScene(layer);
    return layer;
}

void ServerSwitchLayer::onApply(CCObject *)
{
    auto newUrl = input->getString();
    if (newUrl.empty())
    {
        input->setString("https://www.boomlings.com/database");
        newUrl = input->getString();
    }
    Mod::get()->setSavedValue("server", newUrl);
    latestValue = newUrl;
    FLAlertLayer::create(
        "Set URL Successfully", "Server URL has been set to:\n" + newUrl, "OK")
        ->show();
}