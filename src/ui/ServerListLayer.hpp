#pragma once

#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCScene.h"
#include "Geode/ui/ScrollLayer.hpp"
#include "Geode/ui/Scrollbar.hpp"
#include "Types.hpp"

class ServerNode;

class ServerListLayer : public cocos2d::CCLayer {
    protected:
        bool init() override;
        void keyBackClicked() override;

        cocos2d::CCMenu *m_backMenu = nullptr;
        geode::ScrollLayer *m_scroll = nullptr;
        geode::Scrollbar *m_scrollbar = nullptr;

        GDPSTypes::Server m_selectedServer;

        int page = 1;
        int showing = 10;
    public:
        static ServerListLayer *create();
        static cocos2d::CCScene *scene();
        
        void onBack(CCObject *sender);
        void onSelect(GDPSTypes::Server server);

        GDPSTypes::Server getSelected();
};