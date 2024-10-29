#include "../utils/PSUtils.hpp"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include <Geode/ui/ScrollLayer.hpp>
using namespace geode::prelude;

class RecoveryNode : public CCNode {
protected:
  std::string formatFileSize(std::uintmax_t size) {
    const std::vector<std::string> units = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;

    while (size >= 1024 && unitIndex < units.size() - 1) {
      size /= 1024;
      unitIndex++;
    }

    return std::to_string(size) + " " + units[unitIndex];
  }

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

    auto title = CCLabelBMFont::create(filename.c_str(), "chatFont.fnt");
    title->setAnchorPoint({0, 0.5});
    title->limitLabelWidth(300, 1.0, 0.2);
    this->addChildAtPosition(title, Anchor::TopLeft, {7.5, -12});

    auto fileSize =
        CCLabelBMFont::create(formatFileSize(std::filesystem::file_size(
                                                 dirs::getSaveDir() / filename))
                                  .c_str(),
                              "chatFont.fnt");
    fileSize->setAnchorPoint({0, 0.5});
    fileSize->limitLabelWidth(300, 1.0, 0.2);
    this->addChildAtPosition(fileSize, Anchor::BottomLeft, {7.5, 12});

    auto menu = CCMenu::create();
    menu->setContentSize({360, 70});
    this->addChildAtPosition(menu, Anchor::Center);

    auto btn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Use", "goldFont.fnt", "GJ_button_01.png", .75),
        this, menu_selector(RecoveryNode::recover));
    btn->setAnchorPoint({1, 0.5});
    btn->setScale(2.f/3);
    menu->addChildAtPosition(btn, Anchor::BottomRight, {-10, 25});

    auto btn2 = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Ignore", "goldFont.fnt", "GJ_button_06.png", .75),
        this, menu_selector(RecoveryNode::ignore));
    btn2->setAnchorPoint({1, 0.5});
    btn2->setScale(2.f/3);
    menu->addChildAtPosition(btn2, Anchor::BottomRight, {-50, 25});

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

  void ignore(CCObject *) {
    auto servers =
        Mod::get()->getSavedValue<std::vector<std::string>>("ignored-recoveries");
        servers.push_back(filename);
        Mod::get()->setSavedValue("ignored-recoveries", servers);
    }
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
    if (ret->initAnchored(400.f, 280.f)) {
      ret->autorelease();
      return ret;
    }

    delete ret;
    return nullptr;
  }
};
