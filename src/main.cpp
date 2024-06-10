#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include "ServerSwitchLayer.hpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCHttpClient.hpp>
class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}

		log::debug("Hello from my MenuLayer::init hook! This layer has {} children.", this->getChildrenCount());

		auto myButton = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("GJ_deleteServerBtn_001.png"),
			this,
			menu_selector(MyMenuLayer::onMyButton)
		);

		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(myButton);

		myButton->setID("my-button"_spr);

		menu->updateLayout();

		return true;
	}

	void onMyButton(CCObject*) {
		ServerSwitchLayer::scene();
	}
};

class $modify(BoomRedirect, CCHttpClient)
{
	void send(CCHttpRequest* req)
    {
		std::string url = req->getUrl();
		auto newUrl = Mod::get()->getSavedValue<std::string>("server");
		if (newUrl.empty()) {
			newUrl = "http://www.boomlings.com/database";
			Mod::get()->setSavedValue("server", newUrl);
		}
        if (url.starts_with("https://www.boomlings.com/database")) {
			req->setUrl(url.replace(0, 34, newUrl).c_str());
		} else if (url.starts_with("http://www.boomlings.com/database")) {
			req->setUrl(url.replace(0, 33, newUrl).c_str());
		}
		log::info("Sending request to {}", req->getUrl());
        CCHttpClient::send(req);
    }
};
