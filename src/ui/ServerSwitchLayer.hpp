#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

class ServerNode;

using namespace geode::prelude;

class ServerSwitchLayer : public CCLayer
{
protected:
    bool init() override;
    void keyBackClicked() override;

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