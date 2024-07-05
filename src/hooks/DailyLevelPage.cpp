using namespace geode::prelude;

#include <Geode/modify/DailyLevelPage.hpp>
class $modify(dLPage, DailyLevelPage) {
	void onTheSafe(CCObject* h) {
		if (DailyLevelPage::m_type != GJTimedLevelType::Event) {
			DailyLevelPage::onTheSafe(h);
		} else {
			auto lbl = LevelBrowserLayer::scene(GJSearchObject::create(SearchType::EventSafe));
			CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5, lbl));
		}
	}
};