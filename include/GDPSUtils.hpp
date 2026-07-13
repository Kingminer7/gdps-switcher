#ifndef GDPSUTILS_HPP
#define GDPSUTILS_HPP

#include "Types.hpp"
#include <matjson.hpp>
#include <string>
#include <Geode/loader/Dispatch.hpp>
#define MY_MOD_ID "km7dev.gdps-switcher"

namespace GDPSUtils {

    struct CreateServerArgs {
        std::string name;
        std::string url;
        bool modRequired = false;
        std::string saveDir = "";
        matjson::Value customData;
        std::string motd = "";
        geode::Mod* mod = geode::Mod::get();
    };
    inline geode::Result<int> createServer(CreateServerArgs args) GEODE_EVENT_EXPORT(&createServer, (args));
    
    struct UpdateServerArgs {
        int id;
        std::string name;
        std::string url;
        bool modRequired = false;
        std::string saveDir = "";
        matjson::Value customData;
        std::string motd = "";
        geode::Mod* mod = geode::Mod::get();
    };
    inline geode::Result<> updateServer(UpdateServerArgs args) GEODE_EVENT_EXPORT(&updateServer, (args));
    /**
    * @brief Retrieves a list of all servers added by the specified mod.
    * 
    * @return A map the available servers.
    */
    inline geode::Result<std::map<int, GDPSTypes::Server>> getModServers(geode::Mod* mod = geode::Mod::get()) GEODE_EVENT_EXPORT(&getModServers, (mod));
    /**
    * @brief Retrieves the current server.
    *
    * @return The current server.
    */
    inline geode::Result<GDPSTypes::Server> getCurrentServer() GEODE_EVENT_EXPORT(&getCurrentServer, ());

    inline geode::Result<bool> deleteServer(int id, geode::Mod* mod = geode::Mod::get()) GEODE_EVENT_EXPORT(&deleteServer, (id, mod));
};

#endif