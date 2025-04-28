#pragma once

#include "Types.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ServerNode;

class ServerListLayer : public CCLayer {
    protected:
        bool init() override;
        void keyBackClicked() override;

        int m_eePos = 0;
        bool m_isEditing = false;

        CCMenu *m_backMenu = nullptr;
        CCMenu *m_bottomMenu = nullptr;
        ScrollLayer *m_scroll = nullptr;
        Scrollbar *m_scrollbar = nullptr;

        int page = 1;
        int showing = 10;

        virtual void keyDown(enumKeyCodes key) override;
    public:
        static ServerListLayer *create();
        static CCScene *scene();

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
        void onSelect(GDPSTypes::Server server);
        void onKonami();
        void onSettings(CCObject *sender);

        static int m_selectedServer;
        std::map<int, GDPSTypes::Server> m_servers;
};