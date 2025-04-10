#include <Geode/modify/GManager.hpp>
#include "../utils/GDPSMain.hpp"
#include "../utils/DataManager.hpp"
#include "Geode/loader/Dirs.hpp"
#include "Geode/loader/Log.hpp"

class GSGManager : public geode::Modify<GSGManager, GManager> {
    public:
        void setup() {
            auto gdpsm = GDPSMain::get();
            if (gdpsm->isActive()) {
                m_fileName = fmt::format("gdpses/{}/{}", DataManager::urlToFilenameSafe(gdpsm->m_currentServer.url), m_fileName);
            }
            auto dir = geode::dirs::getSaveDir() / "gdpses" / DataManager::urlToFilenameSafe(gdpsm->m_currentServer.url);
            std::error_code ec;
            if (!std::filesystem::exists(dir) && !std::filesystem::create_directory(dir, ec)) {
                geode::log::error("Failed to create directory '{}', data will not save: {}", dir.string(), ec.message());
            }
            GManager::setup();
        }

        void save() {
            geode::log::info("{}", m_fileName);
            GManager::save();
        }
};