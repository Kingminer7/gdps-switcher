#include "ServerInfoManager.hpp"
#include "utils/GDPSMain.hpp"

using namespace geode::prelude;

ServerInfoManager *ServerInfoManager::m_instance = nullptr;

void ServerInfoManager::fetch(GDPSTypes::Server& server) {
    if (server.infoLoaded == false) {
        server.infoLoaded = true;
        if (server.id < 0) return;
        // idk weird stuff
        int id = server.id;
        std::string url = server.url;
        m_listeners[server.id].bind([this, id, url] (utils::web::WebTask::Event* e) {
            if (auto res = e->getValue()) {
                if (res->json().isErr()) {
                    log::warn("Failed to parse info for {}: {}", url, res->json().err());
                } else {
                    auto info = res->json().unwrapOrDefault();
                    GDPSTypes::Server& server = GDPSMain::get()->m_servers[id];
                    server.motd = info["motd"].asString().unwrapOr("No MOTD found.");
                    server.icon = info["icon"].asString().unwrapOr("");
                    // serverData.modPolicy = info["mods"]["policy"].asString().unwrapOr(serverData.modPolicy);
                    // serverData.dependencies = info["mods"]["dependencies"].as<std::map<std::string, std::string>>().unwrapOr(serverData.dependencies);
                    // serverData.modList = info["mods"]["modList"].as<std::vector<std::string>>().unwrapOr(serverData.modList);
                    GDPSMain::get()->save();
                    LoadDataEvent(server).post();
                }
            }
        });

        auto req = web::WebRequest();
        m_listeners[server.id].setFilter(req.get(fmt::format("{}/switcher/getInfo.php", server.url)));
    }
}

GDPSTypes::Server& LoadDataEvent::getServer() const {
    return m_server;
}

ServerInfoManager *ServerInfoManager::get() {
    if (!m_instance) m_instance = new ServerInfoManager;
    return m_instance;
}