#include "ServerInfoManager.hpp"
#include "GDPSMain.hpp"

ServerInfoManager *ServerInfoManager::m_instance = nullptr;

ServerInfoManager *ServerInfoManager::get() {
    if (!m_instance) m_instance = new ServerInfoManager;
    return m_instance;
}

void ServerInfoManager::getInfoForServer(GDPSTypes::Server server, ServerNode *node) {
    if (server.empty()) return;
    auto& sdata = m_listeners[server];
    if (sdata.second) sdata.second->release();
    if (node) {
        sdata.second = node;
        sdata.second->retain();
    }

    server.motd = "test";

    if (sdata.first.getFilter().isNull()) {

        sdata.first.bind([this, server, sdata] (utils::web::WebTask::Event* e) {
            if (auto* res = dynamic_cast<web::WebResponse*>(e->getValue())) {
                if (res->ok()) {
                    if (res->json().isErr()) {
                        log::warn("Failed to parse info for {}: {}", server.url, res->json().err());
                    } else {
                        if (GDPSMain::get()->m_servers.contains(server.id)) {
                            auto info = res->json().unwrapOrDefault();
                            auto& serverData = GDPSMain::get()->m_servers[server.id];

                            serverData.motd = info["motd"].asString().unwrapOr("No MOTD found.");
                            serverData.icon = info["icon"].asString().unwrapOr("");
                            // serverData.modPolicy = info["mods"]["policy"].asString().unwrapOr(serverData.modPolicy);
                            // serverData.dependencies = info["mods"]["dependencies"].as<std::vector<std::string>>().unwrapOr(serverData.dependencies);
                            // serverData.modList = info["mods"]["modList"].as<std::map<std::string, std::string>>().unwrapOr(serverData.modList);

                            if (sdata.second) {
                                sdata.second->updateInfo(serverData);
                                sdata.second->release();
                            }
                            auto servers = GDPSMain::get()->m_servers;
                            if (servers.contains(-1)) {
                                servers.erase(-1);
                            }
                            if (servers.contains(-2)) {
                                servers.erase(-2);
                            }
                            Mod::get()->setSavedValue<std::map<int, GDPSTypes::Server>>("servers", servers);                            
                        }
                    }
                }
            }
        });
        auto req = web::WebRequest();
        sdata.first.setFilter(req.get(fmt::format("{}/switcher/getInfo.php", server.url)));
    }
}