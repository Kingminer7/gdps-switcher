#include "ServerInfoManager.hpp"
#include "GDPSMain.hpp"

ServerInfoManager *ServerInfoManager::m_instance = nullptr;

ServerInfoManager *ServerInfoManager::get() {
    if (!m_instance) m_instance = new ServerInfoManager;
    return m_instance;
}

void ServerInfoManager::getInfoForServer(GDPSTypes::Server server, MDTextArea *area) {
    if (server.empty()) return;
    auto& sdata = m_listeners[server];
    if (sdata.second) sdata.second->release();
    sdata.second = area;
    sdata.second->retain();

    server.motd = "test";

    if (sdata.first.getFilter().isNull()) {

        sdata.first.bind([server, sdata] (utils::web::WebTask::Event* e) {
            if (auto* res = dynamic_cast<web::WebResponse*>(e->getValue())) {
                std::string motd;
                if (res->ok()) {
                    if (res->json().isErr()) {
                        motd = "Failed to parse MOTD.";
                        log::warn("Failed to parse MOTD for {}: {}", server.url, res->json().err());
                    } else {
                        auto info = res->json().unwrapOrDefault();
                        motd = info["motd"].asString().unwrapOr("No MOTD found.");
                    }
                    if (GDPSMain::get()->m_servers.contains(server.id)) {
                        GDPSMain::get()->m_servers[server.id].motd = motd;
                    }
                    if (sdata.second) {
                        sdata.second->setString(motd.c_str());
                        sdata.second->release();
                    }
                }
            }
        });
        auto req = web::WebRequest();
        sdata.first.setFilter(req.get(fmt::format("{}/switcher/getInfo.php", server.url)));
    }
}