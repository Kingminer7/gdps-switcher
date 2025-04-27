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

        bool m_editing = false;

        bool init(GDPSTypes::Server server, CCSize size, ServerListLayer *list);
    public:
        void onSelect(CCObject *sender);
        void onEdit(CCObject *sender);
        void onDelete(CCObject *sender);
        void updateSelected(GDPSTypes::Server server);
        void updateInfo();

        void setEditing(bool editing);
        bool isEditing();

        GDPSTypes::Server getServer();

        bool m_locked = false;

        static ServerNode *create(GDPSTypes::Server server, CCSize size, ServerListLayer *list);
};