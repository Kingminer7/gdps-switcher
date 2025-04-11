#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DragNode;

class DragLayer : public GenericContentLayer {
    protected:
        std::vector<DragNode *> m_nodes;
    public:
        bool m_isX = false;
        float m_gap = 0;

        virtual void addNode(DragNode *node);
        virtual void addNode(DragNode *node, int place);

        virtual void reorder(DragNode *node);
        virtual void reorder(DragNode *node, int place);

        static DragLayer *create(float width, float height);
};

class DragNode : public CCMenu {
    protected:
        bool init(DragLayer *layer, int place = -1);

        bool ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
        void ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
        void ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
        void ccTouchCancelled(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override {
            ccTouchEnded(touch, event);
        }
        void registerWithTouchDispatcher() override;

        CCPoint m_diff;
        DragLayer *m_layer;
    public:
        static DragNode *create(DragLayer *layer);
        static DragNode *create(DragLayer *layer, int place);

        float m_baseScale = 1.f;
};