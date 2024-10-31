#include "../utils/PSUtils.hpp"

using namespace geode::prelude;

void onExit() {
  if (Mod::get()->getRequestedAction() == ModRequestedAction::Disable ||
      Mod::get()->getRequestedAction() == ModRequestedAction::Uninstall ||
      Mod::get()->getRequestedAction() ==
          ModRequestedAction::UninstallWithSaveData ||
      PSUtils::get()->switching == true) {
    log::info("Deleting music library and SFX library");
    if (std::filesystem::exists(dirs::getSaveDir() / "musiclibrary.dat"))
      std::filesystem::remove(dirs::getSaveDir() / "musiclibrary.dat");
    else
      log::info("Music libary was not at {}!", dirs::getSaveDir() / "musiclibrary.dat");
    if (std::filesystem::exists(dirs::getSaveDir() / "sfxlibrary.dat"))
      std::filesystem::remove(dirs::getSaveDir() / "sfxlibrary.dat");
    else
      log::info("SFX libary was not at {}!", dirs::getSaveDir() / "sfxlibrary.dat");
  }
}

const int a = std::atexit(onExit);
