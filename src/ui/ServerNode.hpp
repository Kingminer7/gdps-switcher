#pragma once

#include "ServerListLayer.hpp"
#include "Types.hpp"
#include "../utils/ServerInfoManager.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ServerNode : public CCNode {
    protected:
        GDPSTypes::Server& m_server;
        CCMenu* m_menu = nullptr;
        ServerListLayer* m_listLayer = nullptr;
	    geode::EventListener<EventFilter<LoadDataEvent>> *m_listener = nullptr;
        bool m_editing = false;

        ServerNode(GDPSTypes::Server& server) : m_server(server) {};

        bool init(CCSize size, ServerListLayer *list, bool odd);
    public:
        void onSelect(CCObject* sender);
        void onEdit(CCObject* sender);
        void onDelete(CCObject* sender);
        void updateSelected(GDPSTypes::Server server);
        void updateInfo(LoadDataEvent event);

        void setEditing(bool editing);
        bool isEditing();

        GDPSTypes::Server& getServer();

        bool m_locked = false;

        static ServerNode* create(GDPSTypes::Server& server, CCSize size, ServerListLayer* list, bool odd);
};
