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
  if (!isBoomlings())
    this->server = ServerAPIEvents::registerServer(url, -40);
  // ServerConfigManager::get();
}

ServerAPIEvents::Server PSUtils::getServer() { return server; }