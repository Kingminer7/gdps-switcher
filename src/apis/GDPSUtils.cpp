#if 0

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
        if (server->url == url) {
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
    GDPSMain::get()->m_servers[id] = std::make_shared<GDPSTypes::Server>(std::move(server));
    GDPSMain::get()->save();
    return Ok(id);
}

Result<std::map<int, GDPSTypes::Server>> GDPSUtils::getServers() {
    std::map<int, GDPSTypes::Server> ret;
    for (auto [id, server] : GDPSMain::get()->m_servers) {
        ret[id] = *server.get();
    }
    return Ok(ret);
}

// This gets a Server by copy and not by modifiable reference but I don't know if that is intentional.
Result<GDPSTypes::Server> GDPSUtils::getCurrentServer() {
    auto res = GDPSMain::get()->getCurrentServer();
    if (!res) return Err(res.unwrapErr());
    return Ok(*res.unwrap().get());
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
        if (server->url == url && server->saveDir == saveDir) {
            return Ok(*server.get());
        }
    }
    return Err("Server not found");
}

// This should be a Result<void> but too late to change it.
Result<bool> GDPSUtils::deleteServer(int id) {
    auto res = GDPSMain::get()->deleteServer(id);
    if (!res) {
        return Err(res.unwrapErr());
    }
    return Ok(true);
}

Result<bool> GDPSUtils::switchServer(int id) {
    auto res = GDPSMain::get()->switchServer(id);
    if (!res) {
        return Err(res.unwrapErr());
    }
    return Ok(true);
}

Result<GDPSTypes::Server> GDPSUtils::getServerInfo(int id) {
    auto it = GDPSMain::get()->m_servers.find(id);
    if (it == GDPSMain::get()->m_servers.end()) {
        return Err("Server not found");
    }
    return Ok(*it->second.get());
}

Result<bool> GDPSUtils::setServerInfo(int id, std::string name, std::string url, std::string saveDir) {
    auto res = GDPSMain::get()->setServerInfo(id, name, url, saveDir);
    if (!res) {
        return Err(res.unwrapErr());
    }
    return Ok(true);
}

#endif