#include "../utils/PSUtils.hpp"
#include "Geode/cocos/base_nodes/Layout.hpp"
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
  bool init(const std::string &url, const std::string &filename, bool ignored) {
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
    btn->setScale(2.f / 3);
    btn->m_baseScale = 2.f / 3;
    menu->addChildAtPosition(btn, Anchor::BottomRight, {-10, 25});

    CCMenuItemSpriteExtra *btn2;

    if (ignored) {
      btn2 = CCMenuItemSpriteExtra::create(
          ButtonSprite::create("Delete", "goldFont.fnt", "GJ_button_06.png",
                               .75),
          this, menu_selector(RecoveryNode::del));
    } else {
      btn2 = CCMenuItemSpriteExtra::create(
          ButtonSprite::create("Ignore", "goldFont.fnt", "GJ_button_03.png",
                               .75),
          this, menu_selector(RecoveryNode::ignore));
    }

    btn2->setAnchorPoint({1, 0.5});
    btn2->setScale(2.f / 3);
    btn2->m_baseScale = 2.f / 3;
    menu->addChildAtPosition(btn2, Anchor::BottomRight, {-50, 25});

    return true;
  }

public:
  static RecoveryNode *create(const std::string &url,
                              const std::string &filename, bool ignored) {
    auto ret = new RecoveryNode();
    if (ret->init(url, filename, ignored)) {
      ret->autorelease();
      return ret;
    }

    delete ret;
    return nullptr;
  }

  void recover(CCObject *) {
    geode::createQuickPopup(
        "Are you sure",
        "Are you sure you want to use this save? This will overwrite your "
        "current save.",
        "No", "Yes",
        [this](auto, bool yes) {
          if (yes) {
            PSUtils::get()->recovering.push_back(filename);
            std::pair<std::string, std::string> pr;
            for (std::pair<std::string, std::string> pair : PSUtils::get()->saveRecovery) {
              if (pair.second == filename) {
                pr = pair;
              }
            }
            PSUtils::get()->saveRecovery.remove(pr);
            auto iRs = Mod::get()->getSavedValue<std::vector<std::string>>(
                "ignored-recoveries");
            auto found = std::find(iRs.begin(), iRs.end(), filename);
            if (found != iRs.end()) {
              iRs.erase(found);
              Mod::get()->setSavedValue("ignored-recoveries", iRs);
            }
            auto parent = this->getParent();
            this->removeFromParent();
            parent->updateLayout();
          }
        },
        false)
        ->show();
  }

  void del(CCObject *) {
    geode::createQuickPopup(
        "Are you sure",
        "Are you sure you want to delete this save? You cannot recover it if "
        "so.",
        "No", "Yes",
        [this](auto, bool yes) {
          if (yes) {
            if (std::filesystem::exists(dirs::getSaveDir() / filename))
              std::filesystem::remove(dirs::getSaveDir() / filename);
            auto parent = this->getParent();
            this->removeFromParent();
            parent->updateLayout();
            std::pair<std::string, std::string> pr;
            for (std::pair<std::string, std::string> pair : PSUtils::get()->saveRecovery) {
              if (pair.second == filename) {
                pr = pair;
              }
            }
            PSUtils::get()->saveRecovery.remove(pr);
            auto iRs = Mod::get()->getSavedValue<std::vector<std::string>>(
                "ignored-recoveries");
            auto found = std::find(iRs.begin(), iRs.end(), filename);
            if (found != iRs.end()) {
              iRs.erase(found);
              Mod::get()->setSavedValue("ignored-recoveries", iRs);
            }
          }
        },
        false)
        ->show();
  }

  void ignore(CCObject *) {
    geode::createQuickPopup(
        "Ignore?",
        "Are you sure you want to ignore this save? If you change your mind, "
        "you can review it with the ignored recoveries button in the mod "
        "settings.",
        "No", "Yes",
        [this](auto, bool yes) {
          if (yes) {
            auto iRs = Mod::get()->getSavedValue<std::vector<std::string>>(
                "ignored-recoveries");
            iRs.push_back(filename);
            Mod::get()->setSavedValue("ignored-recoveries", iRs);
            auto parent = this->getParent();
            this->removeFromParent();
            parent->updateLayout();
          }
        },
        false)
        ->show();
  }
};

class RecoveryPopup : public geode::Popup<bool> {
protected:
  bool setup(bool ignored) override {
    this->setTitle("GDPS Save Recovery");

    auto scroll = ScrollLayer::create({360, 220});
    scroll->setID("scroll");
    scroll->ignoreAnchorPointForPosition(false);
    scroll->setZOrder(1);
    auto layout = ColumnLayout::create();
    layout->setGap(0);
    layout->setAxisAlignment(AxisAlignment::End);
    scroll->m_contentLayer->setLayout(layout);
    m_mainLayer->addChildAtPosition(scroll, Anchor::Center, {0, -10});

    auto scrollBg = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    scrollBg->setColor({0, 0, 0});
    scrollBg->setContentSize({360, 220});
    scrollBg->setOpacity(90);
    scrollBg->setID("scroll-bg");
    m_mainLayer->addChildAtPosition(scrollBg, Anchor::Center, {0, -10});

    int nodes = 0;

    auto ignoredV = Mod::get()->getSavedValue<std::vector<std::string>>(
        "ignored-recoveries");
    for (const auto &pair : PSUtils::get()->saveRecovery) {
      if ((std::find(ignoredV.begin(), ignoredV.end(), pair.second) !=
           ignoredV.end()) == ignored &&
          std::find(PSUtils::get()->recovering.begin(),
                    PSUtils::get()->recovering.end(),
                    pair.second) == PSUtils::get()->recovering.end()) {
        auto node = RecoveryNode::create(pair.first, pair.second, ignored);
        node->setAnchorPoint({0.5, 1});
        // scroll->m_contentLayer->addChildAtPosition(node, Anchor::Top,
        //                                            {0, -nodes * 70.f});
        scroll->m_contentLayer->addChild(node);
        nodes++;
      }
    }
    scroll->m_contentLayer->setContentHeight(std::max(nodes * 70 + 70, 220));
    scroll->m_contentLayer->updateLayout();
    scroll->scrollToTop();

    return true;
  }

  void onClose(CCObject *obj) override {
    geode::Popup<bool>::onClose(obj);
    if (PSUtils::get()->recovering.size() > 0) {
      geode::createQuickPopup(
          "Restart?",
          "Restarting is recommended. Any data you save or change now will not "
          "apply to recovered save files.",
          "No", "Yes",
          [](auto, bool yes) {
            if (yes) {
              game::restart();
            }
          },
          false)
          ->show();
    }
  }

public:
  static RecoveryPopup *create(bool ignored) {
    auto ret = new RecoveryPopup();
    if (ret->initAnchored(400.f, 280.f, ignored)) {
      ret->autorelease();
      return ret;
    }

    delete ret;
    return nullptr;
  }
};
