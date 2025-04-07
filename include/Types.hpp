#pragma once

#ifndef GDPSUTILS_HPP
#define GDPSUTILS_HPP

#ifdef GEODE_IS_WINDOWS
    #ifdef GDPSUTILS_EXPORTS
        #define GDPSUTILS_API __declspec(dllexport)
    #else
        #define GDPSUTILS_API __declspec(dllimport)
    #endif
#else
    #define GDPSUTILS_API
#endif

namespace GDPSTypes {
    struct Server {
        std::string name = "";
        std::string url = "";
        // this doesnt get saved
        int serverApiId = -1;

        Server(std::string name, std::string url) : url(url), name(name) {}
        Server() {}
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