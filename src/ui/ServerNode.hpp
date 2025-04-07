#pragma once

#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Types.hpp"

class ServerNode : public cocos2d::CCNode {
    protected:
        GDPSTypes::Server m_server = {"", ""};
    public:
        bool init(GDPSTypes::Server server, cocos2d::CCSize size);

        static ServerNode *create(GDPSTypes::Server server, cocos2d::CCSize size);
};