#pragma once

#include "ServerListLayer.hpp"
#include "Types.hpp"
#include "../utils/GDPSMain.hpp"
#include "../utils/ServerInfoManager.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ServerNode : public CCNode {
    protected:
        std::shared_ptr<GDPSTypes::Server> m_server;
        CCMenu* m_editMenu = nullptr;
        CCMenu* m_useMenu = nullptr;
        ServerListLayer* m_listLayer = nullptr;
	    geode::ListenerHandle m_listener;
        bool m_editing = false;
	    int m_index;

        //ServerNode(GDPSTypes::Server& server);

        bool init(CCSize size, ServerListLayer *list, int index, GDPSTypes::Server& server);
    public:
        void onSelect(CCObject* sender);
        void onEdit(CCObject* sender);
        void onDelete(CCObject* sender);
        void onMove(CCObject* sender);
        void updateSelected(const GDPSTypes::Server& server);
        void updateInfo();

        void setEditing(bool editing);
        bool isEditing();

        std::shared_ptr<GDPSTypes::Server> getServer();

        bool m_locked = false;

        static ServerNode* create(GDPSTypes::Server& server, CCSize size, ServerListLayer* list, int index);
};
