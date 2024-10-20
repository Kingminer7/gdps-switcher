#include "PSUtils.hpp"
#include <km7dev.server_api/include/ServerAPI.hpp>

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
  auto server = ServerAPI::get()->getURLById(this->serverId);
  return server.starts_with("www.boomlings.com/database") ||
         server.starts_with("http://www.boomlings.com/database") ||
         server.starts_with("https://www.boomlings.com/database");
}

void PSUtils::initialize(const std::string &url) {
  if (this->initialized)
    return;
  this->initialized = true;
  if (!isBoomlings())
    this->serverId = ServerAPI::get()->registerURL(url, -40);
  // ServerConfigManager::get();
}

int PSUtils::getId() { return serverId; }