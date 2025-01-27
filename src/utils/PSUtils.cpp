#include "PSUtils.hpp"

PSUtils *PSUtils::get() {
  if (!instance) {
    instance = new PSUtils();
    auto url = Mod::get()->getSavedValue<std::string>("server");
    if (url.empty()) {
      url = "https://www.boomlings.com/database/";
      Mod::get()->setSavedValue("server", url);
    }
    instance->initialize(url);
  }
  return instance;
}

bool PSUtils::isBoomlings() {
  return server.url.starts_with("www.boomlings.com/database") ||
         server.url.starts_with("http://www.boomlings.com/database") ||
         server.url.starts_with("https://www.boomlings.com/database");
}

void PSUtils::initialize(const std::string &url) {
  if (this->initialized)
    return;
  this->initialized = true;
  for (auto str : {"viper.geometry_dash_midnight", "teamtcm.geometry-dash-odyssey", "capeling.geometry-dash-lunar"}) {
    if (Loader::get()->isModLoaded(str)) {
      conflicts.push_back(str);
    }
  }
  if (!conflicts.empty()) return;
  if (!isBoomlings())
    this->server = ServerAPIEvents::registerServer(url, -40);
  // ServerConfigManager::get();
}

std::vector<std::string> PSUtils::getConflicts() {
  return conflicts;
}

ServerAPIEvents::Server PSUtils::getServer() { return server; }