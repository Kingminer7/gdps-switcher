#pragma once

#include "Types.hpp"

#include <Geode/loader/Event.hpp>
 
#include <functional>
#include <string>

class LoadDataEvent : public geode::Event {
protected:
	std::string m_motd;
	std::string m_logo;
	GDPSTypes::Server& server;

public:
LoadDataEvent(GDPSTypes::Server& server, std::string const& motd, std::string const& logo);

    std::string getMotd() const;
    std::string getLogo() const;
    GDPSTypes::Server& getServer() const;
};

class ServerInfoManager {
	public:
		void fetch(GDPSTypes::Server& server);
};
