#include "../utils/GDPSMain.hpp"

using namespace geode::prelude;

$on_mod(DataSaved)
{
  if (
    Mod::get()->getRequestedAction() == ModRequestedAction::Disable ||
    Mod::get()->getRequestedAction() == ModRequestedAction::Uninstall ||
    Mod::get()->getRequestedAction() == ModRequestedAction::UninstallWithSaveData ||
    GDPSMain::get()->m_switching
  ) {
    auto writablePath = std::string(CCFileUtils::sharedFileUtils()->getWritablePath());
    auto musicPath = writablePath + "musiclibrary.dat";
    auto sfxPath = writablePath + "sfxlibrary.dat";

    log::debug("Deleting music library and SFX library");
    log::debug("Cache dir: {}", writablePath);

    if (std::filesystem::exists(musicPath)) {
    std::error_code err;
      std::filesystem::remove(musicPath, err);
      if (err) {
        log::warn("Failed to delete music library at {}: {}", musicPath, err.message());
      }
    } else {
      log::debug("Music library was not at {}!", musicPath);
    }

    if (std::filesystem::exists(sfxPath)) {
      std::error_code err;
      std::filesystem::remove(sfxPath, err);
      if (err) {
        log::warn("Failed to delete SFX library at {}: {}", sfxPath, err.message());
      }
    } else {
      log::debug("SFX library was not at {}!", sfxPath);
    }
  }
}
