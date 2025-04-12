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

void DragLayer::setEnabled(bool enabled) {
    m_enabled = enabled;
}

bool DragLayer::isEnabled() {
    return m_enabled;
}

void DragLayer::reorder(DragNode *node, int place) {
    if (place < 0) return log::warn("Tried reordering node to place {}, cannot add to negative place.", place);
    if (m_nodes.size() < place) return log::warn("Tried reordering node to place {} when there are only {}, it will not be added.", place, m_nodes.size());
    auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
    if (it != m_nodes.end()) {
        int currentPlace = std::distance(m_nodes.begin(), it);
        int newPlace = place;
        m_nodes.erase(it);
        if (place > currentPlace) {
            newPlace -= 1;
        }
        m_nodes.insert(m_nodes.begin() + newPlace, node);
        float y = getContentHeight() - m_nodes[0]->getContentHeight() * m_nodes[0]->getAnchorPoint().y;
        for (auto node : m_nodes) {

            node->stopAllActions();
            node->runAction(CCEaseSineOut::create(CCMoveTo::create(.3f, {node->getPositionX(), y})));
            y -= node->getContentHeight() + m_gap;
        }
        if (currentPlace != place && m_onReorder) m_onReorder(m_nodes);
        else log::info("Didnt move");
    }
}

void DragLayer::onDragStart(std::function<void(DragNode *node)> callback) {
    m_onDragStart = callback;
}

void DragLayer::onDragEnd(std::function<void(DragNode *node)> callback) {
    m_onDragEnd = callback;
}

void DragLayer::onReorder(std::function<void(std::vector<DragNode *>)> callback) {
    m_onReorder = callback;
}

std::function<void(std::vector<DragNode *> nodes)> DragLayer::getReorderCallback() {
    return m_onReorder;
}
std::function<void(DragNode *node)> DragLayer::getDragStartCallback() {
    return m_onDragStart;
}

std::function<void(DragNode *node)> DragLayer::getDragEndCallback() {
    return m_onDragEnd;
}


void DragLayer::removeNode(DragNode *node) {
    auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
    if (it != m_nodes.end()) {
        m_nodes.erase(it);
        float y = getContentHeight() - m_nodes[0]->getContentHeight() * m_nodes[0]->getAnchorPoint().y;
        for (auto node : m_nodes) {
            node->stopAllActions();
            node->runAction(CCEaseSineOut::create(CCMoveTo::create(.3f, {node->getPositionX(), y})));
            y -= node->getContentHeight() + m_gap;
        }
    }
}

void DragLayer::removeAllNodes() {
    m_nodes.clear();
}

void DragLayer::reorder(DragNode *node, CCPoint pos) {
    auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
    if (it != m_nodes.end()) {
        int currentPlace = std::distance(m_nodes.begin(), it);
        int newPlace = 0;

        for (size_t i = 0; i < m_nodes.size(); ++i) {
            if (i != currentPlace) {
                log::info("{}", i);
                log::info("{}", m_nodes[i] == nullptr);
                log::info("{}", m_nodes[i]->getID());
                log::info("{}", m_nodes[i]->getParent() == nullptr);
                if (pos.y > convertToWorldSpace(m_nodes[i]->getPosition()).y) {
                    break;
                }
                log::info("Fish 2");
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
    cocos2d::CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
}

bool DragNode::ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *evt) {
    if (!m_layer->isEnabled()) return false;
    auto space = convertTouchToNodeSpace(touch);
    if (space.x < 0 || space.y < 0 || space.x > m_obContentSize.width || space.y > m_obContentSize.height)
        return false;

    if (auto cb = m_layer->getDragStartCallback()) cb(this);

    m_diff = this->getPosition() - touch->getLocation();

    this->stopAllActions();
    this->runAction(cocos2d::CCEaseSineOut::create(
        cocos2d::CCScaleTo::create(0.3f, m_baseScale * 1.1f)));
    this->setZOrder(this->getZOrder() + 1);
    return true;
}

void DragNode::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *evt) {
    if (auto cb = m_layer->getDragEndCallback()) cb(this);
    this->stopAllActions();
    m_layer->reorder(this, touch->getLocation());
    this->setZOrder(this->getZOrder() - 1);
    this->runAction(cocos2d::CCEaseSineOut::create(
        cocos2d::CCScaleTo::create(0.3f, m_baseScale)));
}

void DragNode::ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *evt) {
    auto pos = touch->getLocation() + m_diff;
    if (m_layer->m_isX) this->setPositionX(pos.x);
    else this->setPositionY(pos.y);
}