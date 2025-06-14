#include "../utils/GDPSMain.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/GManager.hpp>

using namespace geode::prelude;

class $modify(GSGManager, GManager) {

    struct Fields {
        static std::vector<GSGManager*> m_managers;
        std::string m_originalFileName;
    };

    void setup() override {
        if (getUserObject("GDPS_DONT_CHANGE"_spr)) return GManager::setup();
        const auto main = GDPSMain::get();
        if (!main->isActive()) return GManager::setup();
        if (m_fields->m_originalFileName != "") return GManager::setup();
        
        m_fields->m_originalFileName = m_fileName;
        GSGManager::Fields::m_managers.push_back(this);

        auto server = main->m_servers[main->m_currentServer];
        const auto dir = geode::dirs::getSaveDir() / "gdpses" / server.saveDir;

        std::error_code error;
        if (!std::filesystem::exists(dir) && !std::filesystem::create_directory(dir, error)) {
            main->registerIssue(fmt::format("Failed to setup save file: {}", error.message()));
            return log::error("Failed to create directory '{}', data will not save: {}", dir.string(), error.message());
        }

        m_fileName = fmt::format("gdpses/{}/{}", server.saveDir, m_fileName);
        GManager::setup();
    }
    void save() {
        if (GDPSMain::get()->m_shouldSaveGameData) GManager::save();
    }
    static void updateFileName() {
        
        

        for (auto manager : Fields::m_managers) {
            const auto main = GDPSMain::get();
            auto server = main->m_servers[main->m_currentServer];
            const auto dir = geode::dirs::getSaveDir() / "gdpses" / server.saveDir;
            std::error_code error;
            if (!std::filesystem::exists(dir) && !std::filesystem::create_directory(dir, error)) {
                main->registerIssue(fmt::format("Failed to setup save file: {}", error.message()));
                return log::error("Failed to create directory '{}', data will not save: {}", dir.string(), error.message());
            }
            if (main->isActive()) {
                manager->m_fileName = fmt::format("gdpses/{}/{}", server.saveDir, manager->m_fields->m_originalFileName);
            }
        }
    }
};

std::vector<GSGManager*> GSGManager::Fields::m_managers = {};