#include "GDPSMain.hpp"
#include "Types.hpp"

#include <km7dev.server_api/include/ServerAPIEvents.hpp>

#include <Geode/Geode.hpp>
#include "ServerInfoManager.hpp"
#include "../hooks/GManager.hpp"

using namespace geode::prelude;

bool GDPSMain::isActive() const {
    if (!m_issues.empty())
        return false;
    if (isBase())
        return false;
    return true;
}

void GDPSMain::registerIssue(const std::string& issue) {
    m_issues.push_back(issue);
}

std::vector<std::string> GDPSMain::getIssues() {
    return m_issues;
}

void GDPSMain::save() const {
    auto servers = m_servers;
    if (servers.contains(-1)) {
        servers.erase(-1);
    }
    if (servers.contains(-2)) {
        servers.erase(-2);
    }
    Mod::get()->setSavedValue<std::map<int, GDPSTypes::Server>>("servers", servers);
}

geode::Result<> GDPSMain::registerServer(GDPSTypes::Server& server) {
    if (m_servers.contains(server.id)) {
        return geode::Err("Server registery already contains this ID.\nContact developers for help.");
    }

    if (server.saveDir.empty()) {
        server.saveDir = fmt::format("{}", server.id);
    }

    ServerInfoManager::get()->fetch(server);
    return geode::Ok();
}

geode::Result<> GDPSMain::modifyRegisteredServer(GDPSTypes::Server& server) {
    if (!m_servers.contains(server.id)) {
        return geode::Err("Server with this ID does not exist.\nContact developers for help.");
    }
    
    if (server.saveDir.empty()) {
        server.saveDir = fmt::format("{}", server.id);
    }

    auto newSaveDir = server.saveDir;
    if (newSaveDir != m_servers[server.id].saveDir) {
        auto path = geode::dirs::getSaveDir() / "gdpses" / newSaveDir;
        std::error_code errCode;
        bool exists = std::filesystem::exists(path, errCode);

        if (exists) {
            return geode::Err(fmt::format("Save directory \"{}\" already exists!", path));
        }
        if (errCode) {
            return geode::Err("Error checking validity of save directory!");
        }

        std::filesystem::rename(geode::dirs::getSaveDir() / "gdpses" / m_servers[server.id].saveDir, path);
    }

    m_servers[server.id].name = server.name;
    m_servers[server.id].url = server.url;
    m_servers[server.id].saveDir = server.saveDir;

    if (m_currentServer == server.id) {
        ServerAPIEvents::updateServer(m_serverApiId, server.url);
        GSGManager::updateFileNames();
    }

    ServerInfoManager::get()->fetch(server);
    return geode::Ok();
}

geode::Result<> GDPSMain::deleteServer(GDPSTypes::Server& server) {
    m_shouldSaveGameData = false;

    std::filesystem::path serverPath = geode::dirs::getSaveDir() / "gdpses" / server.saveDir;
    std::filesystem::path gdpsesDir = geode::dirs::getSaveDir() / "gdpses";
    std::error_code err;

    if (!std::filesystem::exists(serverPath, err) && err) {
        return geode::Err("Failed to check existence of path: {}\n\nnerd shit: std::error_code message: {}", serverPath, err.message());
    }

    err.clear();
    std::filesystem::path canonicalServerPath;
    canonicalServerPath.clear();
    canonicalServerPath = std::filesystem::canonical(serverPath, err);
    if (err) {
        return geode::Err("Failed to get canonical path for {}: {}", serverPath, err.message());
    }

    if (!canonicalServerPath.string().starts_with(gdpsesDir.string()) || serverPath != gdpsesDir) {
        return geode::Err(
            "Attempted to delete a path outside or equal to the gdpses directory: {}\n\n"
            "To prevent unintentional extra data loss, your save was not deleted - "
            "only saves within {} will be deleted. If you want to delete this data, do it manually.",
            serverPath, gdpsesDir
        );
    }

    log::debug("Deleting {}", serverPath);
    std::filesystem::remove_all(serverPath, err);
    if (err) {
        return geode::Err("Failed to delete save data for {}: {}", server.name, err.message());
    }

    m_servers.erase(server.id);
    this->save();

    return geode::Ok();
}

GDPSMain *GDPSMain::m_instance = nullptr;

void GDPSMain::init() {
    m_servers =
        Mod::get()->getSavedValue<std::map<int, GDPSTypes::Server>>("servers");
    m_currentServer =
        Mod::get()->getSavedValue<int>("current", -2);

    auto base = GDPSTypes::Server{-2, "Built-in Servers", ServerAPIEvents::getBaseUrl(), ".."};
    base.iconIsSprite = true;
    base.icon = "gdlogo.png"_spr;
    base.motd = "Vanilla Geometry Dash servers.";
    // ReSharper disable once CppDFAArrayIndexOutOfBounds
    m_servers[-2] = base;
    const auto &server = m_servers[m_currentServer];
    if (m_currentServer >= 0 && isActive()) {
        m_serverApiId = ServerAPIEvents::registerServer(server.url, -40).id;
    }
}

GDPSMain *GDPSMain::get() {
    if (!m_instance) {
        m_instance = new GDPSMain;
        m_instance->init();
    }
    return m_instance;
}

bool GDPSMain::isBase() const {
    return
        m_currentServer == -2;
}

bool GDPSMain::isBase(std::string url) {
    return 
        url == ServerAPIEvents::getBaseUrl()
    ||  fmt::format("{}/", url) == ServerAPIEvents::getBaseUrl();
}
