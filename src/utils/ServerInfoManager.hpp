#pragma once

#include "Geode/loader/Event.hpp"
#include "Types.hpp"
#include <Geode/utils/web.hpp>
#include <functional>

class ServerInfoManager {
    protected:
        static ServerInfoManager *m_instance;
        std::map<GDPSTypes::Server, geode::EventListener<geode::utils::web::WebTask>> m_listeners;
    public:
        static ServerInfoManager *get();
        void getInfoForServer(GDPSTypes::Server server, std::function<void ()> onComplete);
};