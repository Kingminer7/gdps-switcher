#pragma once

#include "ServerListLayer.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Types.hpp"

class ServerNode : public cocos2d::CCNode {
    protected:
        GDPSTypes::Server m_server;
        cocos2d::CCMenu *m_menu = nullptr;
        ServerListLayer *m_listLayer = nullptr;

        bool init(GDPSTypes::Server server, cocos2d::CCSize size, ServerListLayer *list);
    public:
        void onSelect(CCObject *sender);
        void updateVisual(GDPSTypes::Server server);

        GDPSTypes::Server getServer();

        static ServerNode *create(GDPSTypes::Server server, cocos2d::CCSize size, ServerListLayer *list);
};