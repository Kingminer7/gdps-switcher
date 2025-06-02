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

        EventListener<web::WebTask> m_listener;

        bool m_editing = false;

        bool init(GDPSTypes::Server server, CCSize size, ServerListLayer *list, bool odd);
    public:
        void onSelect(CCObject *sender);
        void onEdit(CCObject *sender);
        void onDelete(CCObject *sender);
        void updateSelected(GDPSTypes::Server server);
        void updateInfo(GDPSTypes::Server server);

        void setEditing(bool editing);
        bool isEditing();

        GDPSTypes::Server getServer();

        bool m_locked = false;

        static ServerNode *create(GDPSTypes::Server server, CCSize size, ServerListLayer *list, bool odd);
};