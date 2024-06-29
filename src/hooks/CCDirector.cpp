#include <Geode/modify/CCDirector.hpp>
#include "../utils/DataManager.hpp"

using namespace geode::prelude;

class $modify(director, CCDirector) {
	bool replaceScene(CCScene* scene)
    {
        if (!CCDirector::replaceScene(scene)) return false;
        if (DataManager::get().isBoomlings()) return true;
        if (scene->getChildren() == nullptr || scene->getChildren()->count() == 0) return true;
        auto layer = scene->getChildren()->objectAtIndex(0);
        auto type = std::string(typeid(*layer).name());
		if (type.find("Globed") != std::string::npos && type.find("Layer") != std::string::npos){
            auto alert = FLAlertLayer::create("GDPS Switcher", "Globed will not work unless the GDPS owner sets up a Globed server.", "Ok");
            alert->m_scene = scene;
            alert->show();
        }
        return true;
    }
};
