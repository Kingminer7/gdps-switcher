#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "../ui/ServerListLayer.hpp"
#include "ui/ColorLabel.hpp"

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
        // log::info("{}", ColorLabel::getColoredText("Test"));
        // log::info("{}", ColorLabel::getColoredText("Te<c>st</c> test"));
        // log::info("{}", ColorLabel::getColoredText("Te<cy>st</c> t<cb>e</c>st"));
        // log::info("{}", ColorLabel::getColoredText("Te<c-00AEFF>st</c> t<h>e</c>st"));
        // addChildAtPosition(ColorLabel::create("Test", {60, 60}), Anchor::Center, {0, 60}, false);
        // addChildAtPosition(ColorLabel::create("Te<c>st</c> test", {60, 60}), Anchor::Center, {0, 40}, false);
        // addChildAtPosition(ColorLabel::create("Te<cy>st</c> t<cb>e</c>st", {60, 60}), Anchor::Center, {0, 20}, false);
        // addChildAtPosition(ColorLabel::create("Te<c-00AEFF>st</c> t<h>e</c>st", {60, 60}), Anchor::Center, {0, 0}, false);
	}
};
