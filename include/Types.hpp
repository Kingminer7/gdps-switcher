#pragma once

#ifndef GDPSTYPES_HPP
#define GDPSTYPES_HPP

#ifdef GEODE_IS_WINDOWS
    #ifdef GDPSSwitcher_EXPORTS
        #define GDPSTYPES __declspec(dllexport)
    #else
        #define GDPSTYPES __declspec(dllimport)
    #endif
#else
    #define GDPSTYPES
#endif

namespace GDPSTypes {
    struct Server {
        int id = -1;
        std::string name = "";
        std::string url = "";
        std::string saveDir = "";

        // Many issues and stuff
        // std::string modPolicy = "blacklist";
        // std::map<std::string, std::string> dependencies = {};
        // std::vector<std::string> modList = {};

        // Doesn't save below this comment
        std::string motd = "No MOTD found.";
        bool iconIsSprite = false;
        std::string icon = "";

        Server(int id, std::string name, std::string url, std::string saveDir) : id(id), url(url), name(name), saveDir(saveDir) {}
        Server() {}

        // Comparison operators
        bool operator==(const Server& other) const {
            return name == other.name && url == other.url;
        }

        bool operator!=(const Server& other) const {
            return !(*this == other);
        }

        bool operator<(const Server& other) const {
            return name < other.name || (name == other.name && url < other.url);
        }

        bool operator>(const Server& other) const {
            return other < *this;
        }

        bool empty() {
            return (name.empty() && url.empty()) || id == -1;
        }
    };
    struct OldServer {
        std::string name = "";
        std::string url = "";
    };
}

template <>
struct matjson::Serialize<GDPSTypes::Server>
{
    static geode::Result<GDPSTypes::Server> fromJson(matjson::Value const &value)
    {
        GDPSTypes::Server server(
            value["id"].asInt().unwrapOr(-1),
            value["name"].asString().unwrapOr("Failed to load name."),
            value["url"].asString().unwrapOr("Failed to load url."),
            value["saveDir"].asString().unwrapOr(value["url"].asString().unwrapOr("Failed to load save directory."))
        );
        // server.dependencies = value["mods"]["dependencies"].as<std::map<std::string, std::string>>().unwrapOr(std::map<std::string, std::string>());
        // server.modPolicy = value["mods"]["policy"].asString().unwrapOr("whitelist");
        // server.modList = value["mods"]["modList"].as<std::vector<std::string>>().unwrapOr(std::vector<std::string>());
        return geode::Ok(server);
    }

    static matjson::Value toJson(GDPSTypes::Server const &value)
    {
        auto obj = matjson::makeObject({
            {"id", value.id},
            {"name", value.name},
            {"url", value.url},
            {"saveDir", value.saveDir},
            // {"mods", matjson::makeObject({
            //     {"dependencies", value.dependencies},
            //     {"policy", value.modPolicy},
            //     {"modList", value.modList}
            // })}
        });
        return obj;
    }
};

template <>
struct matjson::Serialize<std::map<int, GDPSTypes::Server>>
{
    static geode::Result<std::map<int, GDPSTypes::Server>> fromJson(matjson::Value const &value)
    {
        std::map<int, GDPSTypes::Server> ret = {};
        for (auto server : value) {
            auto serv = server.as<GDPSTypes::Server>().unwrapOr(GDPSTypes::Server());
            int id = serv.id;
            ret[id] = serv;
        }
        return geode::Ok(ret);
    }

    static matjson::Value toJson(std::map<int, GDPSTypes::Server> const &value)
    {
        std::vector<matjson::Value> servers;
        for (auto const& [id, server] : value) {
            servers.push_back(matjson::Value(server));
        }
        return matjson::Value(servers);
    }
};

template <>
struct matjson::Serialize<GDPSTypes::OldServer>
{
    static geode::Result<GDPSTypes::OldServer> fromJson(matjson::Value const &value)
    {
        return geode::Ok(GDPSTypes::OldServer{
            value["name"].asString().unwrapOr("Failed to load name."),
            value["url"].asString().unwrapOr("Failed to load url.")
        });
    }

    static matjson::Value toJson(GDPSTypes::OldServer const &value)
    {
        auto obj = matjson::makeObject({{"name", value.name}, {"url", value.url}});
        return obj;
    }
};

#endif