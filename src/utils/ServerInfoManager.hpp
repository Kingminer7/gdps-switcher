#pragma once

#include "Types.hpp"

#include <Geode/loader/Event.hpp>
#include <functional>

class LoadDataEvent : public geode::Event {
	GDPSTypes::Server& m_server;

public:
    LoadDataEvent(GDPSTypes::Server& server) : m_server(server) {}
    GDPSTypes::Server& getServer() const;
};

class ServerInfoManager {
	protected:
		static ServerInfoManager *m_instance;
		std::map<int, geode::EventListener<geode::utils::web::WebTask>> m_listeners;
	public:
		void fetch(GDPSTypes::Server& server);

		static ServerInfoManager *get();
};
