#pragma once

#include "Types.hpp"

#include <Geode/loader/Event.hpp>
#include <functional>

class LoadDataEventData {
private:
    GDPSTypes::Server& m_server;
public:
    LoadDataEventData(GDPSTypes::Server& server) : m_server(server) {}
    GDPSTypes::Server& getServer() const;
};

class LoadDataEvent : public geode::Event<LoadDataEvent, bool(LoadDataEventData*)> {
	using Event::Event;
};

class ServerInfoManager {
	protected:
		static ServerInfoManager *m_instance;
		std::map<int, geode::async::TaskHolder<geode::utils::web::WebResponse>> m_listeners;
	public:
		void fetch(std::weak_ptr<GDPSTypes::Server> server);

		static ServerInfoManager *get();
};
