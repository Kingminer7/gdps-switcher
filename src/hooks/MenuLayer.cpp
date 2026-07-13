#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "../ui/ServerListLayer.hpp"
#include "Geode/ui/Popup.hpp"
#include "Geode/utils/general.hpp"
#include "ui/ColorLabel.hpp"
#include "utils/GDPSMain.hpp"
#include <Geode/ui/PopupManager.hpp>
#include <Types.hpp>

using namespace geode::prelude;

class $modify(GDPSML, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		//crash here is good cuz yeah
		auto currentServer = GDPSMain::get()->getCurrentServer().unwrap(); 
		log::info("Current server: {} ({})", currentServer->name, currentServer->id);
		auto s = matjson::Serialize<GDPSTypes::Server>().toJson(*currentServer.get());
		log::info("Current server json: {}", s.dump(4));


    	if(currentServer->modInfo.has_value() && currentServer->modInfo->modRequired && !Loader::get()->getLoadedMod(currentServer->modInfo->modId)) {
			auto& modId = currentServer->modInfo->modId;
			auto installed = Loader::get()->getInstalledMod(modId);
			if(installed) {
				log::error("The mod {} is required to use the GDPS '{}'. The GDPS will not load.", modId, currentServer->name);
				PopupManager::get().manage(
				geode::createQuickPopup("Missing mod for GDPS",
					fmt::format("The mod {} is required to use the GDPS '{}'. It will be enabled for you on restart.", modId, currentServer->name),
					"Restart", nullptr, [installed](auto, bool enable) {
							installed->enable();
							geode::utils::game::restart(true);
						}
				)).showQueue();
    		}

			else {
				log::error("The mod {} is required to use the GDPS '{}', but it is not installed. The GDPS will not load.", modId, currentServer->name);
				Mod::get()->setSavedValue("current", ServerID::RobTop);
				PopupManager::get().manage(
				geode::createQuickPopup("Missing mod for GDPS",
					fmt::format("The mod {} is required to use the GDPS '{}'. Robtop servers will load on the next startup unless you choose a different GDPS", modId, currentServer->name),
					"Choose GDPS", "Quit Game", [](auto, bool quitgame) {
						if(quitgame) {
							geode::utils::game::restart(true);
						} else {
							cocos2d::CCDirector::get()->pushScene(ServerListLayer::scene());
						}
					}
				)).showQueue();
			}
		}
		
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
			if (!btn) {
                spr->removeFromParent();
                return true;
            }
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
