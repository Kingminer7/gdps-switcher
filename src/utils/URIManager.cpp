#include <camila314.geode-uri/include/GeodeURI.hpp>
#include <Geode/Geode.hpp>
#include "../ui/ServerSwitchLayer.hpp"

$on_mod(Loaded) {
	handleURI("gdps/add", [](std::string const& path) {
		if (path.find(";") == std::string::npos) return FLAlertLayer::create("GDPS Switcher", "Invalid server.", "Ok")->show();
		auto url = path.substr(0, path.find(";"));
		auto name = path.substr(path.find(";") + 1, path.size());
		auto servers  = geode::Mod::get()->getSavedValue<std::vector<ServerSwitchLayer::ServerEntry>>("saved-servers");
		for (const auto& server : servers) {
			if (server.name == name && server.url == url) {
				return FLAlertLayer::create("GDPS Switcher", fmt::format("You already have {} in your saved servers.", name), "Ok")->show();
			}
		}
		geode::createQuickPopup("GDPS Switcher", fmt::format("Do you want to add {} to your saved servers?", name).c_str(), "No", "Yes", [name, url](auto popup, bool yes) {
			if (yes) {
				auto servers  = geode::Mod::get()->getSavedValue<std::vector<ServerSwitchLayer::ServerEntry>>("saved-servers");
				servers.push_back({name, url});
				Mod::get()->setSavedValue("saved-servers", servers);
				FLAlertLayer::create("GDPS Switcher", fmt::format("{} was saved successfully.", name), "Ok")->show();
			}
		});
	});
}
