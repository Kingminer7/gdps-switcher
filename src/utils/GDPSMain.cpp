#include "GDPSMain.hpp"
#include "Types.hpp"
#include <km7dev.server_api/include/ServerAPIEvents.hpp>

bool GDPSMain::isActive() {
    geode::log::info("Issues: {}", m_issues);
    geode::log::info("Is base url?: {}", isBase());
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
        Mod::get()->getSavedValue<std::vector<Server>>("saved-servers");
    auto targetUrl = Mod::get()->getSavedValue<std::string>("server", ServerAPIEvents::getBaseUrl());
    bool found = false;
    if (isBase(targetUrl)) {
        m_currentServer = {
            "Built-in Servers",
            targetUrl
        };
    } else {
        for (const auto &server : m_servers) {
            if (server.url == targetUrl) {
                m_currentServer = server;
                found = true;
                break;
            }
        }
        if (!found) {
            m_currentServer = {"Unknown", targetUrl};
        }
        m_currentServer.serverApiId = ServerAPIEvents::registerServer(targetUrl, -40).id;
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
        m_currentServer.url == ServerAPIEvents::getBaseUrl()
    ||  fmt::format("{}/", m_currentServer.url) == ServerAPIEvents::getBaseUrl();
}

bool GDPSMain::isBase(std::string url) {
    return 
        url == ServerAPIEvents::getBaseUrl()
    ||  fmt::format("{}/", url) == ServerAPIEvents::getBaseUrl();
}