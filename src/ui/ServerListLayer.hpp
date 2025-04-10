#pragma once

#include "Types.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ServerNode;

class ServerListLayer : public CCLayer {
    protected:
        bool init() override;
        void keyBackClicked() override;


        CCMenu *m_backMenu = nullptr;
        ScrollLayer *m_scroll = nullptr;
        Scrollbar *m_scrollbar = nullptr;

        int page = 1;
        int showing = 10;
    public:
        static ServerListLayer *create();
        static CCScene *scene();

        void updateList();
        
        void onBack(CCObject *sender);
        void onAdd(CCObject *sender);
        void onEdit(CCObject *sender);
        void onSelect(GDPSTypes::Server server);

        static GDPSTypes::Server m_selectedServer;
        std::vector<GDPSTypes::Server> m_servers;
};