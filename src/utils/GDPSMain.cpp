#include "GDPSMain.hpp"
#include "Types.hpp"

#include <km7dev.server_api/include/ServerAPIEvents.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;

bool GDPSMain::isActive() {
    if (m_issues.size() > 0)
        return false;
    if (isBase())
        return false;
    return true;
}

void GDPSMain::registerIssue(std::string issue) {
    m_issues.push_back(issue);
}

std::vector<std::string> GDPSMain::getIssues() {
    return m_issues;
}

GDPSMain *GDPSMain::m_instance = nullptr;

void GDPSMain::init() {
    m_servers =
        Mod::get()->getSavedValue<std::map<int, GDPSTypes::Server>>("servers");
    m_currentServer =
        Mod::get()->getSavedValue<int>("current", -2);

    m_servers[-2] = GDPSTypes::Server{-2, "Built-in Servers", ServerAPIEvents::getBaseUrl(), ".."};
    m_servers[-2].iconIsSprite = true;
    m_servers[-2].icon = "gdlogo.png"_spr;
    auto &server = m_servers[m_currentServer];
    // std::map<std::string, std::string> problems;
    // for (auto [modid, version] : server.dependencies) {
    //     log::info("{} is {}", modid, Loader::get()->getInstalledMod("geode.loader")->getSavedValue<bool>("should-load-geode.devtools"));
    //     if (!Loader::get()->isModInstalled(modid)) {
            
    //         problems[modid] = "required for this server but not installed";
    //     } else if (!Loader::get()->getInstalledMod("geode.loader")->getSavedValue<bool>("should-load-geode.devtools")) {
    //         problems[modid] = "required for this server but not enabled";
    //     } else {
    //         auto loadedVer = Loader::get()->getInstalledMod(modid)->getVersion();
    //         auto requiredVerRes = ComparableVersionInfo::parse(version);
    //         if (requiredVerRes.isOk()) {
    //             auto requiredVer = requiredVerRes.unwrap();
    //             auto comp = requiredVer.compareWithReason(loadedVer);
    //             if (comp == VersionCompareResult::TooOld) {
    //                 problems[modid] = "too old for this server";
    //             }
    //         } else {
    //             problems[modid] = "an invalid version";
    //         }
    //     }
    // }
    // for (auto mod : Loader::get()->getAllMods()) {
    //     if (mod->isEnabled() && !server.dependencies.contains(mod->getID())) {
    //         if (server.modPolicy == "blacklist" && std::find(server.modList.begin(), server.modList.end(), mod->getID()) != server.modList.end()) {
    //             problems[mod->getID()] = "not allowed on this server";
    //         } else if (server.modPolicy == "whitelist" && std::find(server.modList.begin(), server.modList.end(), mod->getID()) == server.modList.end()) {
    //             problems[mod->getID()] = "not allowed on this server";
    //         }
    //     }
    // }
    // for (auto [modid, problem] : problems) {
    //     registerIssue(fmt::format("Mod [{}](mod:{}) is {}.", modid, modid, problem));
    // }
    if (m_currentServer >= 0 && isActive()) {
        m_serverApiId = ServerAPIEvents::registerServer(server.url, -40).id;
    }
}

GDPSMain *GDPSMain::get() {
    if (!m_instance) {
        m_instance = new GDPSMain;
        m_instance->init();
    }
    return m_instance;
}

bool GDPSMain::isBase() {
    return
        m_currentServer == -2;
}

bool GDPSMain::isBase(std::string url) {
    return 
        url == ServerAPIEvents::getBaseUrl()
    ||  fmt::format("{}/", url) == ServerAPIEvents::getBaseUrl();
}