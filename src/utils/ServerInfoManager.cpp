#include "ServerInfoManager.hpp"

using namespace geode::prelude;

ServerInfoManager *ServerInfoManager::m_instance = nullptr;

void ServerInfoManager::fetch(GDPSTypes::Server& server) {
	LoadDataEvent(server, "Motd", "https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcSBgNT8zJosfwEyrPM2N4F8pRGfBG80hXwvbuN7cqo9zxb600Jblfujv4E&usqp=CAE&s").post();
}

std::string LoadDataEvent::getMotd() const {
    return m_motd;
}

std::string LoadDataEvent::getLogo() const {
    return m_logo;
}

GDPSTypes::Server& LoadDataEvent::getServer() const {
    return m_server;
}

ServerInfoManager *ServerInfoManager::get() {
    if (!m_instance) m_instance = new ServerInfoManager;
    return m_instance;
}