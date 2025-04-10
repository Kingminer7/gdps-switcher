#pragma once

#include "Geode/loader/Event.hpp"
#include "Geode/ui/MDTextArea.hpp"
#include "Types.hpp"
#include <Geode/utils/web.hpp>

class ServerInfoManager {
    protected:
        static ServerInfoManager *m_instance;
        std::map<GDPSTypes::Server, std::pair<geode::EventListener<geode::utils::web::WebTask>, geode::MDTextArea *>> m_listeners;
    public:
        static ServerInfoManager *get();
        void getInfoForServer(GDPSTypes::Server server, geode::MDTextArea *area);
};
