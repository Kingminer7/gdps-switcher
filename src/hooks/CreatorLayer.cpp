#include "../utils/DataManager.hpp"
#include <Geode/modify/DailyLevelPage.hpp>

using namespace geode::prelude;

#include <Geode/modify/CreatorLayer.hpp>
class $modify(creatorLayer, CreatorLayer) {
	bool init() {
		if (!CreatorLayer::init()) {
			return false;
		}

		if (DataManager::get()->isBoomlings()) return true;

		CCMenu *menu = dynamic_cast<CCMenu*>(this->getChildByID("creator-buttons-menu"));
		if (!menu) {
			return true;
		}
		CCMenuItemSpriteExtra *eventBtn = dynamic_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("event-button"));
		if (!eventBtn) {
			return true;
		}
		auto sprite = CCSprite::createWithSpriteFrameName("GJ_eventBtn_001.png");
	    sprite->setScale(static_cast<CCNode*>(eventBtn->getChildren()->objectAtIndex(0))->getScale());
		eventBtn->setSprite(sprite);

		return true;
	}

	void onEventLevel(CCObject* h) {
		if (DataManager::get()->isBoomlings()) {
			CreatorLayer::onEventLevel(h);
			return;
		}
		auto page = DailyLevelPage::create(GJTimedLevelType::Event);
		page->show();
	}
};
