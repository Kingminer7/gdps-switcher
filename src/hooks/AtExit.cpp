#include "../utils/PSUtils.hpp"

using namespace geode::prelude;

void onExit() {
  if (Mod::get()->getRequestedAction() == ModRequestedAction::Disable ||
      Mod::get()->getRequestedAction() == ModRequestedAction::Uninstall ||
      Mod::get()->getRequestedAction() ==
          ModRequestedAction::UninstallWithSaveData ||
      PSUtils::get()->switching == true) {
    log::info("Deleting music library and SFX library");
    log::info("Cache dir: {}", CCFileUtils::sharedFileUtils()->getWritablePath().c_str());
    if (std::filesystem::exists(CCFileUtils::sharedFileUtils()->getWritablePath().c_str() + "musiclibrary.dat"))
      std::filesystem::remove(CCFileUtils::sharedFileUtils()->getWritablePath().c_str() + "musiclibrary.dat");
    else
      log::info("Music library was not at {}!", CCFileUtils::sharedFileUtils()->getWritablePath().c_str() + "musiclibrary.dat");
    if (std::filesystem::exists(CCFileUtils::sharedFileUtils()->getWritablePath().c_str() + "sfxlibrary.dat"))
      std::filesystem::remove(CCFileUtils::sharedFileUtils()->getWritablePath().c_str() + "sfxlibrary.dat");
    else
      log::info("SFX library was not at {}!", CCFileUtils::sharedFileUtils()->getWritablePath() + "sfxlibrary.dat");
  }
}

const int a = std::atexit(onExit);
