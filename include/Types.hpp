//  This file probably used to be for an API with other mods.
// That functionality is no longer supported.
//
//  Use ServerAPI and it's ServerAPIEvents.hpp header instead.
//
//  ServerAPI uses a different system of server identification
// while GDPS Switcher uses an identification system more in line
// for serialization of basic server information to disk.

#pragma once

#ifndef GDPSTYPES_HPP
#define GDPSTYPES_HPP

#include <ranges>
#include <utility>

namespace GDPSTypes {

    // Issues regarding using server information, such as an invalid URL
    // or an invalid directory for saving info to disk.
    struct ServerInvalidity {
        enum _Invalidity : uint16_t {
            Valid       = 0u,
            UrlEmpty    = 1u << 0,
            NameEmpty   = 1u << 1,
            UrlInvalid  = 1u << 2,
            NameInvalid = 1u << 3,
            DirInvalid  = 1u << 4,
            IDInvalid   = 1u << 5 // Pretty much just for when ID == -1
        }; using I = _Invalidity;

        I value;

        ServerInvalidity(uint16_t val) : value(static_cast<ServerInvalidity::I>(val)) {}
        ServerInvalidity(ServerInvalidity::I val) : value(val) {}

        operator I() const {
            return value;
        }

        // Returns true if value != Valid, may not be the right design.
        operator bool() const {
            return static_cast<bool>(value);
        }

        ServerInvalidity operator|(I rhs) const {
            return ServerInvalidity(
                static_cast<uint16_t>(value) | static_cast<uint16_t>(rhs)
            );
        }

        ServerInvalidity operator&(I rhs) const {
            return ServerInvalidity(
                static_cast<uint16_t>(value) & static_cast<uint16_t>(rhs)
            );
        }

        bool operator==(I rhs) const {
            return static_cast<uint16_t>(value) == static_cast<uint16_t>(rhs);
        }

        bool operator!=(I rhs) const {
            return !(*this == rhs);
        }

        ServerInvalidity& operator|=(I rhs) {
            value = *this | rhs;
            return *this;
        }

        ServerInvalidity& operator&=(I rhs) {
            value = *this & rhs;
            return *this;
        }
    };

    // Get rid of all the -2 and -1 magic numbers everywhere.
    // -2 is the built-in (RobTop) servers and -1 is no server.
    struct ServerID {
        enum _ID : int {
            RobTop = -2,
            None = -1,
            GDPS = 0
        }; using I = _ID;

        I value;

        operator I() const {
            return value;
        }

        operator int() const {
            return static_cast<int>(value);
        }

        bool operator==(int rhs) const {
            return static_cast<int>(value) == rhs;
        }

        bool operator!=(int rhs) const {
            return !(*this == rhs);
        }

        operator bool() const {
            return static_cast<int>(value) >= static_cast<int>(ServerID::GDPS) || value == ServerID::RobTop;
        }

        inline bool isPrivateServer() {
            return static_cast<int>(value) > -1;
        }

        inline bool isGDPS() {
            return isPrivateServer();
        }

        inline bool isRobTop() {
            return value == I::RobTop;
        }

        inline bool isBuiltIn() {
            return isRobTop();
        }

        inline bool isNone() {
            return value == I::None;
        }

        inline bool noServer() {
            return isNone();
        }
    };

    struct Server {
        int id = -1;
        std::string name;
        std::string url;
        std::string saveDir;

        // Many issues and stuff
        // std::string modPolicy = "blacklist";
        // std::map<std::string, std::string> dependencies = {};
        // std::vector<std::string> modList = {};

        // Doesn't save below this comment
        bool infoLoaded = false;
        std::string motd = "A Geometry Dash Private Server.";
        bool iconIsSprite = false;
        std::string icon;

        Server(const int id, std::string name, std::string url, std::string saveDir) : id(id), name(std::move(name)), url(std::move(url)), saveDir(std::move(saveDir)) {}
        Server() = default;
        Server(const Server&) = default;
        Server(Server&&) = default;

        Server& operator=(const Server&) = default;
        Server& operator=(Server&&) = default;

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

        [[nodiscard]] bool empty() const {
            return (name.empty() && url.empty()) || id == -1;
        }
    };
    struct OldServer {
        std::string name;
        std::string url;
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
        for (const auto& server : value) {
            auto serv = server.as<GDPSTypes::Server>().unwrapOr(GDPSTypes::Server());
            int id = serv.id;
            ret[id] = serv;
        }
        return geode::Ok(ret);
    }

    static matjson::Value toJson(std::map<int, GDPSTypes::Server> const &value)
    {
        std::vector<matjson::Value> servers;
        for (const auto &server: value | std::views::values) {
            servers.emplace_back(server);
        }
        return {servers};
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
