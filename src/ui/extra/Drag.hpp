#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DragNode;

class DragLayer : public GenericContentLayer {
    protected:
        std::vector<DragNode *> m_nodes;
        std::function<void(std::vector<DragNode *> nodes)> m_onReorder = nullptr;
        std::function<void(DragNode *node)> m_onDragStart = nullptr;
        std::function<void(DragNode *node)> m_onDragEnd = nullptr;

        bool m_enabled = true;
    public:
        bool m_isX = false;
        float m_gap = 0;

        virtual void addNode(DragNode *node);
        virtual void addNode(DragNode *node, int place);

        virtual void removeNode(DragNode *node);
        virtual void removeAllNodes();

        virtual void reorder(DragNode *node, CCPoint pos);
        virtual void reorder(DragNode *node, int place);

        void setEnabled(bool enabled);
        bool isEnabled();

        void onReorder(std::function<void(std::vector<DragNode *>)> callback);
        void onDragStart(std::function<void(DragNode *node)> callback);
        void onDragEnd(std::function<void(DragNode *node)> callback);
        std::function<void(std::vector<DragNode *> nodes)> getReorderCallback();
        std::function<void(DragNode *node)> getDragStartCallback();
        std::function<void(DragNode *node)> getDragEndCallback();

        static DragLayer *create(float width, float height);
};

class DragNode : public CCNode, public CCTouchDelegate {
    protected:
        bool init(DragLayer *layer, int place = -1);

        bool ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
        void ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
        void ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override;
        void ccTouchCancelled(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) override {
            ccTouchEnded(touch, event);
        }

        void onExit() override;

        CCPoint m_diff;
        DragLayer *m_layer;
    public:
        static DragNode *create(DragLayer *layer);
        static DragNode *create(DragLayer *layer, int place);

        float m_baseScale = 1.f;
        bool m_locked = false;
};