#pragma once

using namespace geode::prelude;

class PSUtils {
private:
  int serverId;
  PSUtils() {}
  inline static PSUtils *instance = nullptr;
  bool initialized = false;

public:
  static PSUtils *get();
  bool isBoomlings();
  void initialize(const std::string &url);
  int getId();
};