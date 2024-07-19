#include <Geode/ui/BasedButtonSprite.hpp>
#include "../ui/ServerSwitchLayer.hpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
class $modify(GDPSSwitchMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}

		auto gdpsswitchButton = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("switchServer.png"_spr, 1, CircleBaseColor::Green, CircleBaseSize::MediumAlt),
			this,
			menu_selector(GDPSSwitchMenuLayer::onGDPSSwitchButton)
		);

		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(gdpsswitchButton);

		gdpsswitchButton->setID("gdps-switch-button"_spr);

		menu->updateLayout();

		return true;
	}

	void onGDPSSwitchButton(CCObject*) {
		auto scene = CCScene::create();
		scene->addChild(ServerSwitchLayer::create());
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, scene));
	}
};
