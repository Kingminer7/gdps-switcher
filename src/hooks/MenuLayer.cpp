#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "../ui/ServerListLayer.hpp"

using namespace geode::prelude;

class $modify(GDPSML, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;
		if (auto menu = this->getChildByID("bottom-menu")) {
			auto spr = CircleButtonSprite::createWithSpriteFrameName(
                		"switchServer.png"_spr,
                                1,
                		CircleBaseColor::Green,
				CircleBaseSize::MediumAlt
			);
			if (!spr) return true;
			auto btn = CCMenuItemSpriteExtra::create(
				spr,
				this,
				menu_selector(GDPSML::onGDPS)
			);
			if (!btn) return true;
			btn->setID("gdps-switcher"_spr);
			menu->addChild(btn);
			menu->updateLayout();
		}
		return true;
	}

	void onGDPS(CCObject* sender) {
		auto transition = CCTransitionFade::create(0.5f, ServerListLayer::scene());
		CCDirector::get()->pushScene(transition);
	}
};
