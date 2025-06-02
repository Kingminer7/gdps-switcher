#include "../utils/GDPSMain.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/GManager.hpp>

using namespace geode::prelude;

class $modify(GSGManager, GManager) {
        void setup() override {
            const auto main = GDPSMain::get();
            auto server = main->m_servers[main->m_currentServer];
            const auto dir = geode::dirs::getSaveDir() / "gdpses" / server.saveDir;
            std::error_code error;
            if (!std::filesystem::exists(dir) && !std::filesystem::create_directory(dir, error)) {
                main->registerIssue(fmt::format("Failed to setup save file: {}", error.message()));
                return log::error("Failed to create directory '{}', data will not save: {}", dir.string(), error.message());
            }
            if (main->isActive()) {
                m_fileName = fmt::format("gdpses/{}/{}", server.saveDir, m_fileName);
            }
            GManager::setup();
        }
};