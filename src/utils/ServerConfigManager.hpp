#pragma once

#include <km7dev.server_api/include/ServerAPI.hpp>
#include "PSUtils.hpp"
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class ServerConfigManager
{
private:
    std::vector<std::string> requiredMods;
    std::vector<std::string> modList; // Banned or allowed, based on isWhitelist
    bool isWhitelist = false;

    EventListener<web::WebTask> m_listener;
    bool hasInitialized;

    ServerConfigManager() {}

    inline static ServerConfigManager *instance = nullptr;

public:
    static ServerConfigManager *get()
    {
        if (!instance) {
            instance = new ServerConfigManager();
            instance->initialize();
        }
        return instance;
    }

    void initialize()
    {
        if (!PSUtils::get()->isBoomlings()) {
            this->m_listener.bind([] (web::WebTask::Event* e) {
                if (web::WebResponse* res = e->getValue()) {
                    log::info("{}", res->string().unwrapOr("Request failed."));
                }
            });

            // structure: server + "endpoint"
            auto url = ServerAPI::get()->getCurrentURL() + "getGeodeConfig.php";
            auto req = web::WebRequest();
            req.bodyString("secret=Wmfd2893gb7");
            req.userAgent(""),
            this->m_listener.setFilter(req.post(url));
        }
    }
};