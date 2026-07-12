
// MUST be defined before including the header.
#include "../utils/GDPSMain.hpp"
#include "Types.hpp"

// Dispatch.hpp is #pragma once, so we must manually redefine
// the macros to the "define" (export) variants here.
#define GEODE_DEFINE_EVENT_EXPORTS
#undef GEODE_EVENT_EXPORT
#define GEODE_EVENT_EXPORT(fnPtr, callArgs) \
    GEODE_EVENT_EXPORT_DEFINE(fnPtr, callArgs, GEODE_EVENT_EXPORT_ID_FOR(#fnPtr, #callArgs))
#undef GEODE_EVENT_EXPORT_NORES
#define GEODE_EVENT_EXPORT_NORES(fnPtr, callArgs) \
    GEODE_EVENT_EXPORT(fnPtr, callArgs)
#include <GDPSUtils.hpp>
using namespace geode::prelude;

Result<int> GDPSUtils::createServer(CreateServerArgs args) {
    int id = 0;
    for (auto &[serverId, server] : GDPSMain::get()->m_servers) {
        if (serverId < 0) continue;
        if (server->url == args.url) {
            return Err("Server already saved as {}", args.name);
        }
        if (serverId == id) id++;
        else break;
    }
    auto server = GDPSTypes::Server();
    server.name = args.name;
    server.url = args.url;
    server.id = id;
    server.saveDir = args.saveDir;
    server.motd = args.motd;
    server.modInfo.emplace(args.mod->getID(), args.modRequired, args.customData);
    auto res = GDPSMain::get()->registerServer(std::make_shared<GDPSTypes::Server>(std::move(server)));
    if (!res) return Err(res.unwrapErr());
    GDPSMain::get()->save();
    return Ok(id);
}

Result<> GDPSUtils::updateServer(UpdateServerArgs args) {
    GDPSTypes::Server server;
    server.id = args.id;
    server.name = args.name;
    server.url = args.url;
    server.saveDir = args.saveDir;
    server.motd = args.motd;
    server.modInfo.emplace(args.mod->getID(), args.modRequired, args.customData);
    return GDPSMain::get()->modifyRegisteredServer(server);
}


Result<std::map<int, GDPSTypes::Server>> GDPSUtils::getModServers(geode::Mod* mod) {
    std::map<int, GDPSTypes::Server> ret;
    for (auto [id, server] : GDPSMain::get()->m_servers) {
        if (!server->modInfo.has_value() || server->modInfo->modId != mod->getID()) continue;
        ret[id] = *server.get();
    }
    return Ok(ret);
}

// This gets a Server by copy and not by modifiable reference but I don't know if that is intentional.
Result<GDPSTypes::Server> GDPSUtils::getCurrentServer() {
    auto res = GDPSMain::get()->getCurrentServer();
    if (!res) return Err(res.unwrapErr());
    return Ok(*res.unwrap().get());
}

// This should be a Result<void> but too late to change it.
Result<bool> GDPSUtils::deleteServer(int id, geode::Mod* mod) {
    auto it = GDPSMain::get()->m_servers.find(id);
    if (it == GDPSMain::get()->m_servers.end()) {
        return Err("Server not found");
    }
    if (!it->second->modInfo.has_value() || it->second->modInfo->modId != mod->getID()) {
        return Err("Server not owned by this mod");
    }
    auto res = GDPSMain::get()->deleteServer(id);
    if (!res) {
        return Err(res.unwrapErr());
    }
    return Ok(true);
}