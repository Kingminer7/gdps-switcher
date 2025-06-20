#pragma once

#include "ServerListLayer.hpp"
#include "Types.hpp"
#include "../utils/ServerInfoManager.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ServerNode : public CCNode {
    protected:
        GDPSTypes::Server m_server;
        CCMenu* m_editMenu = nullptr;
        CCMenu* m_useMenu = nullptr;
        ServerListLayer* m_listLayer = nullptr;
	    geode::EventListener<EventFilter<LoadDataEvent>> m_listener;
        bool m_editing = false;
	int m_index;

        ServerNode(GDPSTypes::Server& server) : m_server(server) {};

        bool init(CCSize size, ServerListLayer *list, int index);
    public:
        void onSelect(CCObject* sender);
        void onEdit(CCObject* sender);
        void onDelete(CCObject* sender);
        void onMove(CCObject* sender);
        void updateSelected(GDPSTypes::Server server);
        void updateInfo();

        void setEditing(bool editing);
        bool isEditing();

        GDPSTypes::Server& getServer();

        bool m_locked = false;

        static ServerNode* create(GDPSTypes::Server& server, CCSize size, ServerListLayer* list, int index);
};
