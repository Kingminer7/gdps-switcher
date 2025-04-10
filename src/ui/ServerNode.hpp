#pragma once

#include "ServerListLayer.hpp"
#include "Types.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ServerNode : public CCNode {
    protected:
        GDPSTypes::Server m_server;
        CCMenu *m_menu = nullptr;
        ServerListLayer *m_listLayer = nullptr;

        bool init(GDPSTypes::Server server, CCSize size, ServerListLayer *list);
    public:
        void onSelect(CCObject *sender);
        void updateVisual(GDPSTypes::Server server);

        GDPSTypes::Server getServer();

        static ServerNode *create(GDPSTypes::Server server, CCSize size, ServerListLayer *list);
};