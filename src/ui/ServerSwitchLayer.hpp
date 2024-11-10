#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

#include <Geode/loader/Event.hpp>
#include <Geode/Result.hpp>
#include <Geode/utils/Task.hpp>

class ServerNode;

using namespace geode::prelude;

class ServerSwitchLayer : public CCLayer
{
protected:
    bool init() override;
    void keyBackClicked() override;
    CCMenuItemSpriteExtra *applyBtn;
    EventListener<Task<Result<std::filesystem::path>>> m_pickListener;
public:
    struct ServerEntry
    {
        std::string name;
        std::string url;
    };

    static ServerSwitchLayer *create();
    static cocos2d::CCLayer *scene();
    void onGoBack(CCObject *);
    void onApply(CCObject *);
    ScrollLayer *scroll;
    void update(std::vector<ServerEntry> const &servers, bool resetPos);
    void saveToList(CCObject *obj);
    void selectServer(ServerNode *node);
    void onNew(CCObject *);
    void importServers(CCObject *);
    void exportServers(CCObject *);
    void onFileOpen(Task<Result<std::filesystem::path>>::Event* event);
    void onFileSave(Task<Result<std::filesystem::path>>::Event* event);
};

template <>
struct matjson::Serialize<ServerSwitchLayer::ServerEntry>
{
    static Result<ServerSwitchLayer::ServerEntry> fromJson(matjson::Value const &value)
    {
        return Ok(ServerSwitchLayer::ServerEntry{
            .name = value["name"].asString().unwrapOr("Failed to load name."),
            .url = value["url"].asString().unwrapOr("Failed to load url.")
        });
    }

    static matjson::Value toJson(ServerSwitchLayer::ServerEntry const &value)
    {
        auto obj = matjson::makeObject({{"name", value.name}, {"url", value.url}});
        return obj;
    }
};