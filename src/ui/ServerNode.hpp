#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include "ServerSwitchLayer.hpp"

using namespace geode::prelude;

class ServerSwitchLayer;

class ServerNode : public CCNode {
protected:
    ServerSwitchLayer* m_layer;
    ServerSwitchLayer::ServerEntry server;
    CCMenuItemSpriteExtra* selectBtn;

    bool init(
        ServerSwitchLayer* layer,
        ServerSwitchLayer::ServerEntry entry,
        CCSize size
    );

public:
    static ServerNode* create(
        ServerSwitchLayer* layer,
        ServerSwitchLayer::ServerEntry entry,
        CCSize size
    );

    void selectServer(CCObject*);
    void deleteServer(CCObject*);
    void editServer(CCObject*);
};