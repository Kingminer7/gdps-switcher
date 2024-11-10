#pragma once

#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

class PSUtils {
private:
  PSUtils() {}
  inline static PSUtils *instance = nullptr;
  bool initialized = false;
public:
  int serverId;
  bool firstML = true;
  bool switching = false;
  static PSUtils *get();
  bool isBoomlings();
  void initialize(const std::string &url);
  int getId();
  std::list<std::pair<std::string, std::string>> saveRecovery = {};
  std::list<std::string> recovering = {};
};
