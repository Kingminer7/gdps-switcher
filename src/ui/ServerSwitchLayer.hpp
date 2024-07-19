#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

#include <Geode/loader/Event.hpp>
#include <Geode/utils/Result.hpp>
#include <Geode/utils/Task.hpp>

class ServerNode;

using namespace geode::prelude;

class ServerSwitchLayer : public CCLayer
{
protected:
    bool init() override;
    void keyBackClicked() override;
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
    static ServerSwitchLayer::ServerEntry from_json(matjson::Value const &value)
    {
        return ServerSwitchLayer::ServerEntry{
            .name = value["name"].as_string(),
            .url = value["url"].as_string()};
    }

    static matjson::Value to_json(ServerSwitchLayer::ServerEntry const &value)
    {
        auto obj = matjson::Object();
        obj["name"] = value.name;
        obj["url"] = value.url;
        return obj;
    }
};