#include "GManager.hpp"
#include "../utils/GDPSMain.hpp"

#ifdef GEODE_IS_IOS
    static constexpr const char* const gdpsesPath = "save/gdpses";
#else
    static constexpr const char* const gdpsesPath = "gdpses";
#endif

std::vector<GSGManager*> GSGManager::Fields::m_managers = {};

void GSGManager::setup() {
    if (getUserObject("DONT_CHANGE_DIR"_spr)) return GManager::setup();
    const auto main = GDPSMain::get();
    if (!main->isActive()) return GManager::setup();
    if (m_fields->m_originalFileName != "") return GManager::setup();
    
    m_fields->m_originalFileName = m_fileName;
    GSGManager::Fields::m_managers.push_back(this);

    auto server = main->m_servers[main->m_currentServer];
    const auto dir = geode::dirs::getSaveDir() / "gdpses" / server.saveDir;

    std::error_code err;
    if (!std::filesystem::exists(dir, err)) {
        if (err) {
            main->registerIssue(fmt::format("Failed to check if directory exists: {}", err.message()));
            return log::error("Failed to check directory '{}', data will not save: {}", dir.string(), err.message());
        }
        if (!std::filesystem::create_directory(dir, err)) {
            main->registerIssue(fmt::format("Failed to setup save file: {}", err.message()));
            return log::error("Failed to create directory '{}', data will not save: {}", dir.string(), err.message());
        }
        if (err) {
            main->registerIssue(fmt::format("Error after creating directory: {}", err.message()));
            return log::error("Error after creating directory '{}', data will not save: {}", dir.string(), err.message());
        }
    }
    m_fileName = fmt::format("{}/{}/{}", gdpsesPath, server.saveDir, m_fileName);
    GManager::setup();
}

void GSGManager::save() {
    if (GDPSMain::get()->m_shouldSaveGameData) GManager::save();
}

void GSGManager::updateFileNames() {
    log::debug("Updating filenames");
    for (auto manager : Fields::m_managers) {
        if (!manager) continue;
        const auto main = GDPSMain::get();
        auto server = main->m_servers[main->m_currentServer];
        const auto dir = geode::dirs::getSaveDir() / "gdpses" / server.saveDir;
        std::error_code err;
        if (!std::filesystem::exists(dir, err)) {
            if (err) {
                main->registerIssue(fmt::format("Failed to check if directory exists: {}", err.message()));
                log::error("Failed to check directory '{}', data will not save: {}", dir.string(), err.message());
                continue;
            }
            if (!std::filesystem::create_directory(dir, err)) {
                main->registerIssue(fmt::format("Failed to create directory: {}", err.message()));
                log::error("Failed to create directory '{}', data will not save: {}", dir.string(), err.message());
                continue;
            }
            if (err) {
                main->registerIssue(fmt::format("Error after creating directory: {}", err.message()));
                log::error("Error after creating directory '{}', data will not save: {}", dir.string(), err.message());
                continue;
            }
        }
        if (main->isActive()) {
            manager->m_fileName = fmt::format("{}/{}/{}", gdpsesPath, server.saveDir, manager->m_fields->m_originalFileName);
        }
    }
}
