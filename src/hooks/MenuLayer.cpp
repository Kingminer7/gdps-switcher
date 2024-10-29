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

	if (PSUtils::get()->firstML == true) {
		PSUtils::get()->firstML = false;
		if (PSUtils::get()->saveRecovery.size() > Mod::get()->getSavedValue<std::vector<std::string>>("ignored-recoveries").size()) {
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
    auto scene = CCScene::create();
    scene->addChild(ServerSwitchLayer::create());
    CCDirector::sharedDirector()->replaceScene(
        CCTransitionFade::create(0.5, scene));
  }
};
