#pragma once

#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Types.hpp"

class ServerNode : public cocos2d::CCNode {
    protected:
        GDPSTypes::Server m_server = {"", ""};
        cocos2d::CCMenu *m_menu = nullptr;

        bool init(GDPSTypes::Server server, cocos2d::CCSize size);
    public:
        void onSelect(CCObject *sender);

        static ServerNode *create(GDPSTypes::Server server, cocos2d::CCSize size);
};