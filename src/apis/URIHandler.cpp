#include <camila314.geode-uri/include/GeodeURI.hpp>
#include <Geode/Geode.hpp>
#include "Types.hpp"
#include "GDPSUtils.hpp"

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// I hope this works lmao
$on_mod(Loaded) {
	handleURI("gdps/add", [](std::string const& path) {
		if (path.find(";") == std::string::npos) return FLAlertLayer::create("GDPS Switcher", "Invalid server.", "Ok")->show();
		auto parts = split(path, ';');
        if (parts.size() < 2) return FLAlertLayer::create("GDPS Switcher", "Invalid server.", "Ok")->show();
        auto name = parts[0];
        auto url = parts[1];
        auto saveDir = parts.size() > 2 ? parts[2] : "";
        auto id = GDPSUtils::createServer(name, url, saveDir);
        if (id.isErr()) {
            return FLAlertLayer::create("GDPS Switcher", "Failed to add server.", "Ok")->show();
        }
        FLAlertLayer::create("GDPS Switcher", "Server added successfully.", "Ok")->show();
	});
}