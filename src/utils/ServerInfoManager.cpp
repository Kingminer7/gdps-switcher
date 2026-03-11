#include "ServerInfoManager.hpp"
#include "utils/GDPSMain.hpp"

using namespace geode::prelude;

ServerInfoManager *ServerInfoManager::m_instance = nullptr;

void ServerInfoManager::fetch(std::weak_ptr<GDPSTypes::Server> server) {
    auto serverLock = server.lock();
    if (!serverLock) return;
    if (serverLock->infoLoaded) return;
    serverLock->infoLoaded = true;
    if (serverLock->id < 0) return;
    // idk weird stuff
    int id = serverLock->id;
    std::string url = serverLock->url;
    
    auto req = web::WebRequest();
    std::string endpoint = serverLock->url;
    if (!endpoint.empty() && endpoint.back() != '/')
        endpoint += '/';
    endpoint += "switcher/getInfo.php";

    m_listeners[serverLock->id].spawn(
        req.get(endpoint),
        [this, server, url = std::move(url)] (web::WebResponse value) {
            auto serverLock = server.lock();
            if (!serverLock) {
                log::warn("Server object for ({}) destroyed before fetch could finish", url);
                return;
            }
            auto json = value.json();
            if (json.isErr()) {
                // This little guy right here explodes GD if run.
                // I don't know why or how but the json.err() into the format args causes a bad alloc
                //log::warn("Failed to parse info for {}: {}", server.url, json.err());
                return;
            }
            auto info = json.unwrap();
            serverLock->motd = info["motd"].asString().unwrapOr("No MOTD found.");
            serverLock->icon = info["icon"].asString().unwrapOr("");
            // serverData.modPolicy = info["mods"]["policy"].asString().unwrapOr(serverData.modPolicy);
            // serverData.dependencies = info["mods"]["dependencies"].as<std::map<std::string, std::string>>().unwrapOr(serverData.dependencies);
            // serverData.modList = info["mods"]["modList"].as<std::vector<std::string>>().unwrapOr(serverData.modList);
            auto ev = LoadDataEventData(*serverLock.get());
            LoadDataEvent().send(&ev);
        }
    );
}

GDPSTypes::Server& LoadDataEventData::getServer() const {
    return m_server;
}

ServerInfoManager *ServerInfoManager::get() {
    if (!m_instance) m_instance = new ServerInfoManager;
    return m_instance;
}