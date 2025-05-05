#pragma once

#include "Types.hpp"
#include "ui/ServerNode.hpp"

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class ServerInfoManager {
    protected:
        static ServerInfoManager *m_instance;
        std::map<GDPSTypes::Server, std::pair<EventListener<web::WebTask>, ServerNode *>> m_listeners;
    public:
        static ServerInfoManager *get();
        void getInfoForServer(GDPSTypes::Server server, ServerNode *node);
};
