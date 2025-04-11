#include "Drag.hpp"

DragLayer *DragLayer::create(float width, float height) {
    auto ret = new DragLayer();
    if (ret->initWithColor({0, 0, 0, 0}, width, height)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void DragLayer::addNode(DragNode *node, int place) {
    if (place < 0) return log::warn("Tried adding node at place {}, cannot add to negative place.", place);
    if (m_nodes.size() < place) return log::warn("Tried adding node to place {} when there are only {}, it will not be added.", place, m_nodes.size());
    m_nodes.insert(m_nodes.begin() + place, node);
}

void DragLayer::addNode(DragNode *node) {
    addNode(node, m_nodes.size());
}

void DragLayer::reorder(DragNode *node, int place) {
    if (place < 0) return log::warn("Tried reordering node to place {}, cannot add to negative place.", place);
    if (m_nodes.size() < place) return log::warn("Tried reordering node to place {} when there are only {}, it will not be added.", place, m_nodes.size());
    auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
    if (it != m_nodes.end()) {
        log::info("Moving {} from {} to {}", node->getID(), std::distance(m_nodes.begin(), it), place);
        m_nodes.erase(it);
        m_nodes.insert(m_nodes.begin() + place, node);
        float y = getContentHeight() - m_nodes[0]->getContentHeight() * m_nodes[0]->getAnchorPoint().y;
        for (auto node : m_nodes) {
            log::info("{}", y);
            node->runAction(CCEaseSineOut::create(CCMoveTo::create(.3f, {node->getPositionX(), y})));
            y -= node->getContentHeight() + m_gap;
        }
    }
}

void DragLayer::reorder(DragNode *node) {
    auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
    if (it != m_nodes.end()) {
        int currentPlace = std::distance(m_nodes.begin(), it);
        int newPlace = 0;

        for (size_t i = 0; i < m_nodes.size(); ++i) {
            if (i != currentPlace) {
                auto otherNode = m_nodes[i];
                auto nodeWorldPosY = node->getParent()->convertToWorldSpace(node->getPosition()).y;
                auto otherNodeWorldPosY = otherNode->getParent()->convertToWorldSpace(otherNode->getPosition()).y;
    
                log::info("Node {} world position: {}", node, nodeWorldPosY);
                log::info("OtherNode {} world position: {}", otherNode, otherNodeWorldPosY);
                if (node->getParent()->convertToWorldSpace(node->getPosition()).y > otherNode->getParent()->convertToWorldSpace(otherNode->getPosition()).y) {
                    break;
                }
                newPlace = i + 1;
            }
        }
        reorder(node, newPlace);
    }
}


bool DragNode::init(DragLayer *layer, int place) {
    if (!CCMenu::init()) return false;
    m_layer = layer;
    m_bIgnoreAnchorPointForPosition = false;
    if (place == -1) m_layer->addNode(this);
    else m_layer->addNode(this, place);
    return true;
}

DragNode *DragNode::create(DragLayer *layer) {
    auto ret = new DragNode();
    if (ret->init(layer)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

DragNode *DragNode::create(DragLayer *layer, int place) {
    auto ret = new DragNode();
    if (ret->init(layer, place)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void DragNode::registerWithTouchDispatcher() {
    cocos2d::CCTouchDispatcher::get()->addTargetedDelegate(this, -10710955, true);
}

bool DragNode::ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *evt) {
    auto space = convertTouchToNodeSpace(touch);
    if (space.x < 0 || space.y < 0 || space.x > m_obContentSize.width || space.y > m_obContentSize.height)
        return false;

    m_diff = this->getPosition() - touch->getLocation();

    this->stopAllActions();
    this->runAction(cocos2d::CCEaseSineOut::create(
        cocos2d::CCScaleTo::create(0.3f, m_baseScale * 1.1f)));
    this->setZOrder(this->getZOrder() + 1);
    return true;
}

void DragNode::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *evt) {
    this->stopAllActions();
    this->runAction(cocos2d::CCEaseSineOut::create(
        cocos2d::CCScaleTo::create(0.3f, m_baseScale)));
    this->setZOrder(this->getZOrder() - 1);

    m_layer->reorder(this);
}

void DragNode::ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *evt) {
    auto pos = touch->getLocation() + m_diff;
    if (m_layer->m_isX) this->setPositionX(pos.x);
    else this->setPositionY(pos.y);
}