#pragma once

using namespace geode::prelude;

class PSUtils {
private:
  int serverId;
  PSUtils() {}
  inline static PSUtils *instance = nullptr;
  bool initialized = false;
public:
  bool firstML = true;
  static PSUtils *get();
  bool isBoomlings();
  void initialize(const std::string &url);
  int getId();
  std::list<std::pair<std::string, std::string>> saveRecovery = {};
  std::list<std::string> recovering = {};
};