#include "ServerSwitchLayer.hpp"
#include "ServerNode.hpp"
#include "ServerPopup.hpp"

#include <Geode/utils/cocos.hpp>

std::string current;

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
    title->setPosition(winSize.width / 2, winSize.height - 22.5f);
    title->setID("title-text");
    this->addChild(title);

    auto menu = CCMenu::create();
    menu->setID("menu");
    menu->setZOrder(10);

    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this, menu_selector(ServerSwitchLayer::onGoBack));
    backBtn->setPosition(-winSize.width / 2 + 25.f, winSize.height / 2 - 25.f);
    backBtn->setID("back-button");
    menu->addChild(backBtn);

    auto applyBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", .8f),
        this, menu_selector(ServerSwitchLayer::onApply));

    applyBtn->setPosition(0.f, -winSize.height / 2 + 25.f);
    applyBtn->setID("apply-button");
    applyBtn->setOpacity(50);
    menu->addChild(applyBtn);

    this->addChild(menu);

    auto scrollBg = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    scrollBg->setColor({0, 0, 0});
    scrollBg->setOpacity(90);
    scrollBg->setContentSize({winSize.width - 85, winSize.height - 100});
    scrollBg->setPosition(winSize / 2);
    scrollBg->ignoreAnchorPointForPosition(false);
    scrollBg->setID("server-scroll-bg");
    this->addChild(scrollBg);

    scroll = ScrollLayer::create({winSize.width - 85, winSize.height - 100});
    scroll->setPosition((winSize - scroll->getContentSize()) / 2);
    scroll->setID("server-scroll");
    this->addChild(scroll);

    auto servers = Mod::get()->getSavedValue<std::vector<ServerEntry>>("saved-servers");
    current = Mod::get()->getSavedValue<std::string>("server");
    update(servers, true);

    auto cornerMenu = CCMenu::create();
    cornerMenu->setID("corner-menu");
    cornerMenu->setPosition(winSize.width - 19, 51.f);
    cornerMenu->setZOrder(10);
    cornerMenu->setScale(.75f);
    cornerMenu->setLayout(ColumnLayout::create()->setAxisAlignment(AxisAlignment::Start));
    cornerMenu->setContentSize({50.f, 130.f});
    this->addChild(cornerMenu);

    auto newBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_newBtn_001.png"),
        this, menu_selector(ServerSwitchLayer::onNew));
    newBtn->setID("new-button");
    cornerMenu->addChild(newBtn);

    auto importBtn = CCMenuItemSpriteExtra::create(
        CircleButtonSprite::createWithSpriteFrameName("import.png"_spr, 1.0f, CircleBaseColor::Pink, CircleBaseSize::Big),
        this, menu_selector(ServerSwitchLayer::importServers));
    importBtn->setID("import-button");
    cornerMenu->addChild(importBtn);

    auto exportBtn = CCMenuItemSpriteExtra::create(
        CircleButtonSprite::createWithSpriteFrameName("export.png"_spr, 1.0f, CircleBaseColor::Green, CircleBaseSize::Big),
        this, menu_selector(ServerSwitchLayer::exportServers));
    exportBtn->setID("export-button");
    cornerMenu->addChild(exportBtn);

    cornerMenu->updateLayout();

    return true;
}

void ServerSwitchLayer::importServers(CCObject *)
{
    geode::createQuickPopup(
        "Import Servers",
        "This will overwrite your current server list. Are you sure you want to continue?",
        "No", "Yes",
        [this](auto, bool yes)
        {
            if (yes)
            {
                file::FilePickOptions options = {
                    std::nullopt,
                    {{.description = "Server Lists",
                      .files = {"*.json"}}}};

                m_pickListener.bind(this, &ServerSwitchLayer::onFileOpen);
                m_pickListener.setFilter(file::pick(file::PickMode::OpenFile, options));
            }
        });
}

void ServerSwitchLayer::onFileOpen(Task<Result<std::filesystem::path>>::Event *event)
{
    if (event->isCancelled())
    {
        return;
    }
    if (auto result = event->getValue())
    {
        if (!result->isOk())
        {
            FLAlertLayer::create(
                "Error",
                fmt::format("Failed to open file. Error: {}", result->err().value()),
                "Ok")
                ->show();
            return;
        }
        std::ifstream inputFile(result->unwrap());
        if (!inputFile.is_open())
        {
            FLAlertLayer::create(
                "Error",
                "Failed to open file.",
                "Ok")
                ->show();
            return;
        }
        std::string file_contents;
        std::string line;
        while (std::getline(inputFile, line))
        {
            file_contents += line;
        }

        inputFile.close();

            std::string err;
            auto opt = matjson::parse(file_contents, err);
            auto data = opt.value_or("err");
            if (data == "err") {
                FLAlertLayer::create(
                    "Error",
                    "Invalid file format.",
                    "Ok")
                    ->show();
                return;
            }
            auto servers = data["servers"].as<std::vector<ServerEntry>>();
            Mod::get()->setSavedValue("saved-servers", servers);
            update(servers, true);
            FLAlertLayer::create(
                "Success",
                "Servers imported successfully.",
                "Ok")
                ->show();
    }
}

void ServerSwitchLayer::exportServers(CCObject *)
{
    file::FilePickOptions options = {
        std::nullopt,
        {{.description = "JSON Files",
          .files = {"*.json"}}}};

    m_pickListener.bind(this, &ServerSwitchLayer::onFileSave);
    m_pickListener.setFilter(file::pick(file::PickMode::SaveFile, options));
}

void ServerSwitchLayer::onFileSave(Task<Result<std::filesystem::path>>::Event *event)
{
    if (event->isCancelled())
    {
        return;
    }
    if (auto result = event->getValue())
    {
        if (!result->isOk())
        {
            FLAlertLayer::create(
                "Error",
                fmt::format("Failed to save file. Error: {}", result->err().value()),
                "Ok")
                ->show();
            return;
        }
        auto path = result->unwrap();
        if (path.extension() != ".json")
        {
            path += ".json";
        }
        
        std::ofstream outputFile(path);
        if (!outputFile.is_open())
        {
            FLAlertLayer::create(
                "Error",
                "Failed to save file.",
                "Ok")
                ->show();
            return;
        }
        auto servers = Mod::get()->getSavedValue<std::vector<ServerSwitchLayer::ServerEntry>>("saved-servers");
        outputFile << matjson::format_as(matjson::Object{{"servers", servers}});
        outputFile.close();
        FLAlertLayer::create(
            "Success",
            "Servers exported successfully.",
            "Ok")
            ->show();
    }
}

void ServerSwitchLayer::onNew(CCObject *)
{
    ServerPopup::create(this, nullptr)->show();
}

void ServerSwitchLayer::keyBackClicked()
{
    this->onGoBack(nullptr);
}

void ServerSwitchLayer::onGoBack(CCObject *)
{
    if (current == Mod::get()->getSavedValue<std::string>("server"))
    {
        CCDirector::get()->replaceScene(CCTransitionFade::create(0.5, MenuLayer::scene(false)));
    }
    else
    {
        geode::createQuickPopup(
            "Exit without saving?",      
            "You have unsaved changes.", 
            "No", "Yes",                 
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

void ServerSwitchLayer::onApply(CCObject *)
{
    update(Mod::get()->getSavedValue<std::vector<ServerEntry>>("saved-servers"), false);
    Mod::get()->setSavedValue("server", current);
    geode::createQuickPopup(
        "Restart to apply",
        "You must restart the game for changes to take effect.",
        "Later", "Restart",
        [](auto, bool btn2)
        {
            if (btn2)
            {
                game::restart();
            }
        });
}

void ServerSwitchLayer::selectServer(ServerNode *node)
{
    current = node->getServer().url;
    update(Mod::get()->getSavedValue<std::vector<ServerSwitchLayer::ServerEntry>>("saved-servers"), false);
}

void ServerSwitchLayer::update(std::vector<ServerEntry> const &servers, bool resetPos)
{
    scroll->m_contentLayer->removeAllChildren();
    float totalHeight = 0.f;
    std::vector<ServerNode *> rendered;

    auto node = ServerNode::create(this, {"Geometry Dash servers", "https://www.boomlings.com/database/"}, {scroll->m_contentLayer->getContentSize().width, 45}, "https://www.boomlings.com/database/" == current);
    node->preventDelete();
    node->setPosition(0, totalHeight);
    scroll->m_contentLayer->addChild(node);
    rendered.push_back(node);
    totalHeight += 50;

    for (auto &server : servers)
    {
        auto node = ServerNode::create(this, server, {scroll->m_contentLayer->getContentSize().width, 45}, server.url == current);
        node->setPosition(0, totalHeight);
        scroll->m_contentLayer->addChild(node);
        totalHeight += 50;
        rendered.push_back(node);
    }
    totalHeight -= 5;

    if (totalHeight < scroll->m_contentLayer->getContentSize().height)
    {
        totalHeight = scroll->m_contentLayer->getContentSize().height;
    }

    for (auto &node : rendered)
    {
        node->setPositionY(totalHeight - node->getPositionY() - 45);
    }

    scroll->m_contentLayer->setContentSize({scroll->m_contentLayer->getContentWidth(), totalHeight});

    if (resetPos)
    {
        scroll->moveToTop();
    }
}
