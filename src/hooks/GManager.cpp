#include "../utils/GDPSMain.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/GManager.hpp>

using namespace geode::prelude;

class GSGManager : public geode::Modify<GSGManager, GManager> {
    public:
        void setup() {
            auto gdpsm = GDPSMain::get();
            auto server = gdpsm->m_servers[gdpsm->m_currentServer];
            if (gdpsm->isActive()) {
                m_fileName = fmt::format("gdpses/{}/{}", server.saveDir, m_fileName);
            }
            auto dir = geode::dirs::getSaveDir() / "gdpses" / server.saveDir;
            std::error_code ec;
            if (!std::filesystem::exists(dir) && !std::filesystem::create_directory(dir, ec)) {
                geode::log::error("Failed to create directory '{}', data will not save: {}", dir.string(), ec.message());
            }
            GManager::setup();
        }

        void save() {
            GManager::save();
        }
};