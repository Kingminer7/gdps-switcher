#include "PSUtils.hpp"
#include "km7dev.server_api/include/ServerAPIEvents.hpp"

PSUtils *PSUtils::get() {
  if (!instance) {
    instance = new PSUtils();
    auto url = Mod::get()->getSavedValue<std::string>("server", ServerAPIEvents::getBaseUrl());
    instance->initialize(url);
  }
  return instance;
}

bool PSUtils::isBase() {
  return server.url == ServerAPIEvents::getBaseUrl();
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
  if (url != ServerAPIEvents::getBaseUrl())
    this->server = ServerAPIEvents::registerServer(url, -40);
  // ServerConfigManager::get();
}

std::vector<std::string> PSUtils::getConflicts() {
  return conflicts;
}

ServerAPIEvents::Server PSUtils::getServer() { return server; }
