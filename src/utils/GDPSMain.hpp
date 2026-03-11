#pragma once

#include "Types.hpp"
#include <km7dev.server_api/include/ServerAPIEvents.hpp>

#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>

namespace ServerID {
    constexpr int RobTop = -2;
    constexpr int None = -1;
}

class GDPSMain {
    friend class GSGManager;
    protected:
        static GDPSMain *m_instance;
        geode::ListenerHandle m_ServerAPIUpdateListener;
        std::vector<std::string> m_issues = {};
        void init();
        [[nodiscard]] geode::Result<> setServerSaveDir(std::shared_ptr<GDPSTypes::Server> server, std::string_view saveDir); // Caller is responsible for managing save state.
    public:
        bool isActive() const;
        void registerIssue(const std::string& issue);
        std::vector<std::string> getIssues();
        bool isBase() const;
        geode::Result<std::shared_ptr<GDPSTypes::Server>> getCurrentServer();
        geode::Result<std::shared_ptr<GDPSTypes::Server>> getServer(int id);
        geode::Result<> setServerInfo(int id, std::string_view name = "", std::string_view url = "", std::string_view saveDir = "");
        geode::Result<> registerServer(std::shared_ptr<GDPSTypes::Server> server);
        geode::Result<> modifyRegisteredServer(const GDPSTypes::Server& server);
        geode::Result<> deleteServer(std::shared_ptr<GDPSTypes::Server> server);
        geode::Result<> deleteServer(int id);
        geode::Result<> switchServer(int id);
        GDPSTypes::ServerInvalidity isValidServer(const GDPSTypes::Server& server) const; // Will check if server info can be saved to disk.
        bool serverExists(int id) const;
        bool shouldSaveGameData() const;
        static bool isBase(std::string url);

        int currentServer() const;

        void save() const;

        static GDPSMain *get();

        // One of these days I will make this shit private without breaking everything.
        std::map<int, std::shared_ptr<GDPSTypes::Server>> m_servers;
        int m_currentServer = ServerID::RobTop;
        int m_serverApiId = 0;
        bool m_shouldSaveGameData = true;
        bool m_switching = false;
};