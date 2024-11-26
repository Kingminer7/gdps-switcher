#pragma once

#include <Geode/loader/Mod.hpp>
#include <km7dev.server_api/include/ServerAPIEvents.hpp>

using namespace geode::prelude;

class PSUtils {
private:
  PSUtils() {}
  inline static PSUtils *instance = nullptr;
  bool initialized = false;
  ServerAPIEvents::Server server;
public:
  bool firstML = true;
  bool switching = false;
  static PSUtils *get();
  bool isBoomlings();
  void initialize(const std::string &url);
  ServerAPIEvents::Server getServer();
  std::list<std::pair<std::string, std::string>> saveRecovery = {};
  std::list<std::string> recovering = {};
};
