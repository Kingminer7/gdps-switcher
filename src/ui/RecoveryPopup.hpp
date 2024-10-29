#include "../utils/PSUtils.hpp"
#include <Geode/ui/ScrollLayer.hpp>
using namespace geode::prelude;

class RecoveryNode : public CCNode {
protected:
    std::string filename;
  bool init(const std::string &url, const std::string &filename) {
    if (!CCNode::init()) {
      return false;
    }
    this->filename = filename;

    this->setContentSize({360, 70});

    auto bg = CCScale9Sprite::create("GJ_square02.png", {0, 0, 80, 80});
    bg->setContentSize({360, 70});
    this->addChildAtPosition(bg, Anchor::Center);

    auto title = CCLabelBMFont::create(url.c_str(), "chatFont.fnt");
    title->setAnchorPoint({0, 0.5});
    title->limitLabelWidth(300, 1.0, 0.2);
    this->addChildAtPosition(title, Anchor::TopLeft, {7.5, -10});

    auto menu = CCMenu::create();
    this->addChild(menu);

    auto btn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Use", "chatFont.fnt", "GJ_button_04.png", 1.0),
        this, menu_selector(RecoveryNode::recover));
    btn->setAnchorPoint({1, 0.5});
    menu->addChildAtPosition(btn, Anchor::TopRight);

    return true;
  }

public:
  static RecoveryNode *create(const std::string &url,
                              const std::string &filename) {
    auto ret = new RecoveryNode();
    if (ret->init(url, filename)) {
      ret->autorelease();
      return ret;
    }

    delete ret;
    return nullptr;
  }

    void recover(CCObject *) {
        PSUtils::get()->recovering.push_back(filename);
        this->removeFromParent();
    }
};

class RecoveryPopup : public geode::Popup<> {
protected:
  bool setup() override {
    this->setTitle("GDPS Save Recovery");

    auto scroll = ScrollLayer::create({360, 220});
    scroll->setID("scroll");
    scroll->ignoreAnchorPointForPosition(false);
    scroll->setZOrder(1);
    m_mainLayer->addChildAtPosition(scroll, Anchor::Center, {0, -10});

    auto scrollBg = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    scrollBg->setColor({0, 0, 0});
    scrollBg->setContentSize({360, 220});
    scrollBg->setOpacity(90);
    scrollBg->setID("scroll-bg");
    m_mainLayer->addChildAtPosition(scrollBg, Anchor::Center, {0, -10});

    int nodes = 0;

    for (const auto &pair : PSUtils::get()->saveRecovery) {
      auto node = RecoveryNode::create(pair.first, pair.second);
      node->setAnchorPoint({0.5, 1});
      scroll->m_contentLayer->addChildAtPosition(node, Anchor::Top,
                                                 {0, -nodes * 70.f});
      nodes++;
    }

    return true;
  }

public:
  static RecoveryPopup *create() {
    auto ret = new RecoveryPopup();
    if (ret->initAnchored(700.f, 280.f)) {
      ret->autorelease();
      return ret;
    }

    delete ret;
    return nullptr;
  }
};