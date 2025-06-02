#pragma once

#include "Types.hpp"

#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/ui/ScrollLayer.hpp"

class ServerNode;

class ServerListLayer : public cocos2d::CCLayer {
    protected:
        bool init() override;
        void keyBackClicked() override;

        int m_konamiPos = 0;
        bool m_isEditing = false;

        cocos2d::CCMenu *m_backMenu = nullptr;
        cocos2d::CCMenu *m_bottomMenu = nullptr;
        geode::ScrollLayer *m_scroll = nullptr;

        int page = 1;
        int showing = 10;

        void keyDown(cocos2d::enumKeyCodes key) override;
    public:
        static ServerListLayer *create();
        static cocos2d::CCScene *scene();

        void updateList();

        bool ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
        void ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
        void ccTouchCancelled(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override {
            ccTouchEnded(touch, event);
        }
        void registerWithTouchDispatcher() override;
        void onExit() override;
        
        void onBack(CCObject *sender);
        void onAdd(CCObject *sender);
        void onEdit(CCObject *sender);
        void onSelect(const GDPSTypes::Server &server) const;
        void onKonami();
        void onSettings(CCObject *sender);

        static int m_selectedServer;
        std::map<int, GDPSTypes::Server> m_servers;
};