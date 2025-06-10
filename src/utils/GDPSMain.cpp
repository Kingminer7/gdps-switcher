#include "GDPSMain.hpp"
#include "Types.hpp"

#include <km7dev.server_api/include/ServerAPIEvents.hpp>

#include <Geode/Geode.hpp>

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
