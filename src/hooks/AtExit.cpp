#include "../utils/PSUtils.hpp"

using namespace geode::prelude;

void onExit() {
  if (Mod::get()->getRequestedAction() == ModRequestedAction::Disable ||
      Mod::get()->getRequestedAction() == ModRequestedAction::Uninstall ||
      Mod::get()->getRequestedAction() ==
          ModRequestedAction::UninstallWithSaveData ||
      PSUtils::get()->switching == true) {
    log::info("Deleting music library and SFX library");
    log::info("Cache dir: {}", std::string(CCFileUtils::sharedFileUtils()->getWritablePath()));
    if (std::filesystem::exists(std::string(CCFileUtils::sharedFileUtils()->getWritablePath()) + "musiclibrary.dat"))
      std::filesystem::remove(std::string(CCFileUtils::sharedFileUtils()->getWritablePath()) + "musiclibrary.dat");
    else
      log::info("Music library was not at {}!", std::string(CCFileUtils::sharedFileUtils()->getWritablePath()) + "musiclibrary.dat");
    if (std::filesystem::exists(std::string(CCFileUtils::sharedFileUtils()->getWritablePath()) + "sfxlibrary.dat"))
      std::filesystem::remove(std::string(CCFileUtils::sharedFileUtils()->getWritablePath()) + "sfxlibrary.dat");
    else
      log::info("SFX library was not at {}!", std::string(CCFileUtils::sharedFileUtils()->getWritablePath()) + "sfxlibrary.dat");
  }
}

const int a = std::atexit(onExit);
