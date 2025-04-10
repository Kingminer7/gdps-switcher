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
        std::string name = "";
        std::string url = "";

        // Doesn't save below
        std::string motd = "No MOTD found.";
        int serverApiId = -1;

        Server(std::string name, std::string url) : url(url), name(name) {}
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

        // idc that geode docs want me to use bool const instead, johnny cena
        bool empty() {
            return name == "" && url == "";
        }
    };
}

template <>
struct matjson::Serialize<GDPSTypes::Server>
{
    static geode::Result<GDPSTypes::Server> fromJson(matjson::Value const &value)
    {
        return geode::Ok(GDPSTypes::Server(
            value["name"].asString().unwrapOr("Failed to load name."),
            value["url"].asString().unwrapOr("Failed to load url.")
        ));
    }

    static matjson::Value toJson(GDPSTypes::Server const &value)
    {
        auto obj = matjson::makeObject({{"name", value.name}, {"url", value.url}});
        return obj;
    }
};

#endif