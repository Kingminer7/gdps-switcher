#include "../utils/GDPSMain.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/MusicDownloadManager.hpp>

using namespace geode::prelude;

class $modify(GSMDM, MusicDownloadManager) {

    gd::string pathForSFXFolder(int p0) {
        gd::string ret = MusicDownloadManager::pathForSFXFolder(p0);
        log::info("{}", ret);
        return ret;
    }

    gd::string pathForSongFolder(int p0) {
        gd::string ret = MusicDownloadManager::pathForSongFolder(p0);
        log::info("{}", ret);
        return ret;
    }
};