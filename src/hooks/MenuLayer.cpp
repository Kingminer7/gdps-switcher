#include "../ui/ServerSwitchLayer.hpp"
#include "../ui/RecoveryPopup.hpp"
#include "../utils/PSUtils.hpp"
#include <Geode/ui/BasedButtonSprite.hpp>

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
class $modify(GDPSSwitchMenuLayer, MenuLayer) {

  bool init() {
    if (!MenuLayer::init()) {
      return false;
    }

    auto gdpsswitchButton = CCMenuItemSpriteExtra::create(
        CircleButtonSprite::createWithSpriteFrameName(
            "switchServer.png"_spr, 1, CircleBaseColor::Green,
            CircleBaseSize::MediumAlt),
        this, menu_selector(GDPSSwitchMenuLayer::onGDPSSwitchButton));

    auto menu = this->getChildByID("bottom-menu");
    menu->addChild(gdpsswitchButton);

    gdpsswitchButton->setID("gdps-switch-button"_spr);

    menu->updateLayout();
    if (!PSUtils::get()->getConflicts().empty()) {
        return true;
    }
	if (PSUtils::get()->firstML == true) {
		PSUtils::get()->firstML = false;
		bool allIgnored = true;
		auto ignored = Mod::get()->getSavedValue<std::vector<std::string>>("ignored-recoveries");
                for (std::pair<std::string, std::string> save : PSUtils::get()->saveRecovery) {
                      if (std::find(ignored.begin(), ignored.end(), save.second) == ignored.end()) {
			    allIgnored = false;
		      }
		}
		
		if (allIgnored == false) {
			Loader::get()->queueInMainThread([this] {
			geode::createQuickPopup(
			"Save Recovery",
			"Potentially lost saves from an older version of GDPS Switcher were found. Do you want to review them?",
			"No",
			"Yes",
			[this](auto, bool yes) {
				if (yes) {
					RecoveryPopup::create(false)->show();
				}
			}, false)->show();
			});
		}
	}

    return true;
  }

  void onGDPSSwitchButton(CCObject *) {
    auto conf = PSUtils::get()->getConflicts();
    if (!conf.empty()) {
        std::string result;
        for (auto c : conf) {
	    result.append("\n");
            result.append(c);
        }
        FLAlertLayer::create("GDPS Switcher", fmt::format("GDPS Switcher is disabled while the following mod{} enabled: {}\nDisable them to use GDPS Switcher.", conf.size() == 1 ? " is" : "s are", result).c_str(), "Ok")->show();
        return;
    }
    auto scene = CCScene::create();
    scene->addChild(ServerSwitchLayer::create());
    CCDirector::sharedDirector()->replaceScene(
        CCTransitionFade::create(0.5, scene));
  }
};
