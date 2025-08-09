#pragma once

#include "Types.hpp"

#include <vector>
#include <string>

class GDPSMain {
    protected:
        static GDPSMain *m_instance;
        std::vector<std::string> m_issues = {};
        void init();
    public:
        bool isActive() const;
        void registerIssue(const std::string& issue);
        std::vector<std::string> getIssues();
        bool isBase() const;
        geode::Result<> registerServer(GDPSTypes::Server& server);
        geode::Result<> modifyRegisteredServer(GDPSTypes::Server& server);
        geode::Result<> deleteServer(GDPSTypes::Server& server);
        static bool isBase(std::string url);

        void save() const;

        static GDPSMain *get();

        std::map<int, GDPSTypes::Server> m_servers;
        int m_currentServer = -2;
        int m_serverApiId = 0;
        bool m_shouldSaveGameData = true;
        bool m_switching = false;
};