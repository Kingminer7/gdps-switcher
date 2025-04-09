#include "ServerInfoManager.hpp"
#include "Geode/loader/Log.hpp"

ServerInfoManager *ServerInfoManager::m_instance = nullptr;

ServerInfoManager *ServerInfoManager::get() {
    if (!m_instance) m_instance = new ServerInfoManager;
    return m_instance;
}

void ServerInfoManager::getInfoForServer(GDPSTypes::Server server, std::function<void ()> onComplete) {
    if (server.url == "") return;
    m_listeners[server].bind([server] (geode::utils::web::WebTask::Event* e) {
        if (auto* res = dynamic_cast<geode::utils::web::WebResponse*>(e->getValue())) {
            if (res->ok()) {
                auto motd = res->json().unwrapOrDefault()["motd"].asString().unwrapOr("No MOTD provided.");
                geode::log::info("{}", motd);
            } else {
                geode::log::warn("Failed to load MOTD for {} - {}", server.url, res->string().unwrapOrDefault());
            }
        }
    });

    auto req = geode::utils::web::WebRequest();
    geode::log::info("{}",m_listeners[server].getFilter().isNull());
    m_listeners[server].setFilter(req.get(fmt::format("{}/switcher/getInfo.php", server.url)));
}