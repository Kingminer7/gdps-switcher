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
        CCPoint m_start;

        int page = 1;
        int showing = 10;

        virtual void keyDown(enumKeyCodes key) override;
    public:
        static ServerListLayer *create();
        static CCScene *scene();

        void updateList(bool scroll = true, bool editMode = false);

        bool ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
        void ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
        void ccTouchCancelled(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override {
            ccTouchEnded(touch, event);
        }
        void registerWithTouchDispatcher() override;

        void onBack(CCObject *sender);
        void onAdd(CCObject *sender);
        void onEdit(CCObject *sender);
        void onSelect(GDPSTypes::Server server);
        void onKonami();

        static GDPSTypes::Server m_selectedServer;
        std::vector<GDPSTypes::Server> m_servers;

        CCMenu *m_backMenu = nullptr;
        ScrollLayer *m_scroll = nullptr;
        Scrollbar *m_scrollbar = nullptr;
};