#include "../utils/GDPSMain.hpp"

using namespace geode::prelude;

$on_mod(DataSaved) {
  if (Mod::get()->getRequestedAction() == ModRequestedAction::Disable ||
      Mod::get()->getRequestedAction() == ModRequestedAction::Uninstall ||
      Mod::get()->getRequestedAction() ==
          ModRequestedAction::UninstallWithSaveData ||
      GDPSMain::get()->m_switching == true) {
    log::debug("Deleting music library and SFX library");
    log::debug("Cache dir: {}",
              std::string(CCFileUtils::sharedFileUtils()->getWritablePath()));
    if (std::filesystem::exists(
            std::string(CCFileUtils::sharedFileUtils()->getWritablePath()) +
            "musiclibrary.dat"))
      std::filesystem::remove(
          std::string(CCFileUtils::sharedFileUtils()->getWritablePath()) +
          "musiclibrary.dat");
    else
      log::debug("Music library was not at {}!",
                std::string(CCFileUtils::sharedFileUtils()->getWritablePath()) +
                    "musiclibrary.dat");
    if (std::filesystem::exists(
            std::string(CCFileUtils::sharedFileUtils()->getWritablePath()) +
            "sfxlibrary.dat"))
      std::filesystem::remove(
          std::string(CCFileUtils::sharedFileUtils()->getWritablePath()) +
          "sfxlibrary.dat");
    else
      log::debug("SFX library was not at {}!",
                std::string(CCFileUtils::sharedFileUtils()->getWritablePath()) +
                    "sfxlibrary.dat");
  }
}