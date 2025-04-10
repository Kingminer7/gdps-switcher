#include "ServerInfoManager.hpp"
#include "Geode/loader/Log.hpp"
#include "GDPSMain.hpp"
#include "Geode/ui/MDTextArea.hpp"

ServerInfoManager *ServerInfoManager::m_instance = nullptr;

ServerInfoManager *ServerInfoManager::get() {
    if (!m_instance) m_instance = new ServerInfoManager;
    return m_instance;
}

void ServerInfoManager::getInfoForServer(GDPSTypes::Server server, geode::MDTextArea *area) {
    if (server.empty()) return;
    auto& sdata = m_listeners[server];
    sdata.second = area;

    server.motd = "test";

    if (sdata.first.getFilter().isNull()) {

        sdata.first.bind([server, sdata] (geode::utils::web::WebTask::Event* e) {
            if (auto* res = dynamic_cast<geode::utils::web::WebResponse*>(e->getValue())) {
                std::string motd;
                if (res->ok()) {
                    if (res->json().isErr()) {
                        motd = "Failed to parse MOTD.";
                        geode::log::warn("Failed to parse MOTD for {}: {}", server.url, res->json().err());
                    } else {
                        auto info = res->json().unwrapOrDefault();
                        motd = info["motd"].asString().unwrapOr("No MOTD found.");
                    }
                    auto serverIt = std::find_if(
                        GDPSMain::get()->m_servers.begin(),
                        GDPSMain::get()->m_servers.end(),
                        [&server](const auto& srv) { return srv.url == server.url; }
                    );
                    if (serverIt != GDPSMain::get()->m_servers.end()) {
                        serverIt->motd = motd;
                    }
                    if (sdata.second) sdata.second->setString(motd.c_str());
                }
            }
        });
        auto req = geode::utils::web::WebRequest();
        sdata.first.setFilter(req.get(fmt::format("{}/switcher/getInfo.php", server.url)));
    }
}