#ifndef GDPSUTILS_HPP
#define GDPSUTILS_HPP

#include "Types.hpp"
#include <matjson.hpp>
#include <string>
#include <Geode/loader/Dispatch.hpp>
#define MY_MOD_ID "km7dev.gdps-switcher"

namespace GDPSUtils {
    /**
    * @brief Creates a new server with a specified name, url, and optionally a save directory.
    * 
    * @param name The name of the server.
    * @param url The URL of the server.
    * @param saveDir Save directory for the server, don't pass for default.
    * @param modRequired Whether the server requires the mod to work. if true, the GDPS will not load if the mod is not loaded.
    * @return The id of the server.
    */

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
    * @brief Retrieves a list of all available servers.
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
    /**
    * @brief Sets the current server.
    *
    * @param id The id of the server to set as current.
    * @return True if the server was set successfully.
    */


    inline geode::Result<bool> deleteServer(int id, geode::Mod* mod = geode::Mod::get()) GEODE_EVENT_EXPORT(&deleteServer, (id, mod));
    /**
    * @brief Change the current server to the one with the specified id. Requires restart.
    *
    * @param id The id of the server to set as current.
    * @return True if the server was set successfully.
    */
};

#endif