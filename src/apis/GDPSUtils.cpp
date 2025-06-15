// MUST be defined before including the header.
#define GEODE_DEFINE_EVENT_EXPORTS
#include <GDPSUtils.hpp>
#include "../ui/ServerListLayer.hpp"
#include "../utils/GDPSMain.hpp"
#include <km7dev.server_api/include/ServerAPIEvents.hpp>
#include "../utils/ServerInfoManager.hpp"
using namespace geode::prelude;

Result<int> GDPSUtils::createServer(std::string name, std::string url, std::string saveDir) {
    int id = 0;
    for (auto &[serverId, server] : GDPSMain::get()->m_servers) {
        if (serverId < 0) continue;
        if (server.url == url) {
            return Err("Server already saved as {}", name);
        }
        if (serverId == id) id++;
        else break;
    }
    auto server = GDPSTypes::Server();
    server.name = name;
    server.url = url;
    server.id = id;
    server.saveDir = saveDir.empty() ? fmt::format("{}", id) : saveDir;
    GDPSMain::get()->m_servers[id] = server;
    GDPSMain::get()->save();
    return Ok(id);
}

Result<std::map<int, GDPSTypes::Server>> GDPSUtils::getServers() {
    return Ok(GDPSMain::get()->m_servers);
}

Result<GDPSTypes::Server> GDPSUtils::getCurrentServer() {
    auto it = GDPSMain::get()->m_servers.find(GDPSMain::get()->m_currentServer);
    if (it == GDPSMain::get()->m_servers.end()) {
        return Err("Current server not found");
    }
    return Ok(it->second);
}

Result<bool> GDPSUtils::setCurrentServer(int id) {
    auto it = GDPSMain::get()->m_servers.find(id);
    if (it == GDPSMain::get()->m_servers.end()) {
        return Err("Server not found");
    }
    GDPSMain::get()->m_currentServer = id;
    return Ok(true);
}

Result<GDPSTypes::Server> GDPSUtils::findServer(std::string url, std::string saveDir) {
    for (auto &[id, server] : GDPSMain::get()->m_servers) {
        if (server.url == url && server.saveDir == saveDir) {
            return Ok(server);
        }
    }
    return Err("Server not found");
}

Result<bool> GDPSUtils::deleteServer(int id) {
    auto it = GDPSMain::get()->m_servers.find(id);
    if (it == GDPSMain::get()->m_servers.end()) {
        return Err("Server not found");
    }
    auto main = GDPSMain::get();
    main->m_shouldSaveGameData = false;
    auto serverPath = geode::dirs::getSaveDir() / "gdpses" / it->second.saveDir;
    auto gdpsesDir = geode::dirs::getSaveDir() / "gdpses";
    if (std::filesystem::exists(serverPath)) {
        if (std::filesystem::canonical(serverPath).string().starts_with(gdpsesDir.string()) && serverPath != gdpsesDir) {
            log::debug("Deleting {}", serverPath);
            std::error_code err;
            std::filesystem::remove_all(serverPath, err);
            if (err) {
                log::warn("Failed to delete {}: {}", serverPath.string(), err.message());
                return Err(fmt::format("Failed to delete save directory {}: {}", serverPath.string(), err.message()));
            }
        } else {
            log::warn("Attempted to delete a path outside or equal to the gdpses directory: {}", serverPath);
            return Err(fmt::format("Save directory {} is outside or equal to the gdpses directory and cannot be deleted automatically.", serverPath.string()));
        }
    }
    if (ServerListLayer::m_selectedServer == it->second.id) {
        ServerListLayer::m_selectedServer = -2;
        Mod::get()->setSavedValue("current", -2);
        main->m_shouldSaveGameData = false;
    }
    main->m_servers.erase(it);
    main->save();
    return Ok(true);
}

Result<bool> GDPSUtils::switchServer(int id) {
    auto it = GDPSMain::get()->m_servers.find(id);
    if (it == GDPSMain::get()->m_servers.end()) {
        return Err("Server not found");
    }
    GDPSMain::get()->m_currentServer = id;
    return Ok(true);
}

Result<GDPSTypes::Server> GDPSUtils::getServerInfo(int id) {
    auto it = GDPSMain::get()->m_servers.find(id);
    if (it == GDPSMain::get()->m_servers.end()) {
        return Err("Server not found");
    }
    return Ok(it->second);
}

Result<bool> GDPSUtils::setServerInfo(int id, std::string name, std::string url, std::string saveDir) {
    auto gdpsMain = GDPSMain::get();
    auto it = gdpsMain->m_servers.find(id);
    if (it == gdpsMain->m_servers.end()) {
        return Err("Server not found");
    }

    auto &server = it->second;

    if (!saveDir.empty() && saveDir != server.saveDir) {
        auto newSaveDirPath = geode::dirs::getSaveDir() / "gdpses" / saveDir;
        auto oldSaveDirPath = geode::dirs::getSaveDir() / "gdpses" / server.saveDir;

        if (std::filesystem::exists(newSaveDirPath)) {
            auto gdpsesDir = geode::dirs::getSaveDir() / "gdpses";
            if (!std::filesystem::canonical(newSaveDirPath).string().starts_with(gdpsesDir.string()) || std::filesystem::equivalent(newSaveDirPath, gdpsesDir)) {
                log::warn("{} already exists and is not part of the gdpses subdirectory or is the gdpses directory itself - will not delete.", newSaveDirPath.string());
                return Err("Save directory already exists and cannot be overwritten.");
            }
            std::error_code err;
            std::filesystem::remove_all(newSaveDirPath, err);
            if (err) {
                log::warn("Failed to delete {}: {}", newSaveDirPath.string(), err.message());
                return Err(fmt::format("Failed to delete existing save directory {}: {}", newSaveDirPath.string(), err.message()));
            }
        }

        if (std::filesystem::exists(oldSaveDirPath)) {
            std::error_code err;
            std::filesystem::rename(oldSaveDirPath, newSaveDirPath, err);
            if (err) {
                log::warn("Failed to rename {} to {}: {}", oldSaveDirPath.string(), newSaveDirPath.string(), err.message());
                return Err(fmt::format("Failed to rename save directory from {} to {}: {}", oldSaveDirPath.string(), newSaveDirPath.string(), err.message()));
            }
        }

        server.saveDir = saveDir;
    }

    if (!name.empty()) {
        server.name = name;
    }

    if (!url.empty()) {
        server.url = url;
    }

    if (gdpsMain->m_currentServer == id) {
        ServerAPIEvents::updateServer(gdpsMain->m_serverApiId, server.url);
    }

    ServerInfoManager::get()->fetch(server);
    gdpsMain->save();

    return Ok(true);
}