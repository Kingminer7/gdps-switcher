#include "GDPSMain.hpp"
#include "Types.hpp"

#include <km7dev.server_api/include/ServerAPIEvents.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;

bool GDPSMain::isActive() {
    if (m_issues.size() > 0)
        return false;
    if (isBase())
        return false;
    return true;
}

void GDPSMain::registerIssue(std::string issue) {
    m_issues.push_back(issue);
}

GDPSMain *GDPSMain::m_instance = nullptr;

void GDPSMain::init() {
    m_servers =
        Mod::get()->getSavedValue<std::map<int, GDPSTypes::Server>>("servers");
    m_currentServer =
        Mod::get()->getSavedValue<int>("current", -2);

    m_servers[-2] = GDPSTypes::Server{-2, "Built-in Servers", ServerAPIEvents::getBaseUrl(), ".."};
    m_servers[-2].iconIsSprite = true;
    m_servers[-2].icon = "gdlogo.png"_spr;
    if (m_currentServer >= 0) {
        m_serverApiId = ServerAPIEvents::registerServer(m_servers[m_currentServer].url, -40).id;
    }
    
    for (const auto& [id, server] : m_servers) {
        log::info("Server: {} {} {} {}", id, server.name, server.url, server.saveDir);
    }
}

GDPSMain *GDPSMain::get() {
    if (!m_instance) {
        m_instance = new GDPSMain;
        m_instance->init();
    }
    return m_instance;
}

bool GDPSMain::isBase() {
    return
        m_currentServer == -2;
}

bool GDPSMain::isBase(std::string url) {
    return 
        url == ServerAPIEvents::getBaseUrl()
    ||  fmt::format("{}/", url) == ServerAPIEvents::getBaseUrl();
}