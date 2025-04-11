#pragma once

#include "Types.hpp"
#include "ServerListLayer.hpp"
#include "extra/Drag.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ServerEditNode;

class EditServersPopup : public Popup<ServerListLayer *> {
    protected:
        bool setup(ServerListLayer *layer) override;      
    public:
        void updateList();
        virtual void onExit() override;

        static EditServersPopup* create(ServerListLayer *layer);

        ServerListLayer *m_listLayer = nullptr;
        ScrollLayer *m_scroll = nullptr;  
};

class ServerEditNode : public DragNode {
    protected:
        GDPSTypes::Server m_server;
        CCMenu *m_menu = nullptr;
        CCLabelBMFont *m_name = nullptr;
        EditServersPopup *m_popup = nullptr;

        bool init(GDPSTypes::Server server, EditServersPopup *popup);
    public:
        void onDelete(CCObject *sender);
        void onEdit(CCObject *sender);

        GDPSTypes::Server getServer();

        static ServerEditNode *create(GDPSTypes::Server server, EditServersPopup *popup);
};