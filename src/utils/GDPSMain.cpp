#include "GDPSMain.hpp"
#include "Types.hpp"

#include <Geode/Geode.hpp>
#include "ServerInfoManager.hpp"
#include "../hooks/GManager.hpp"
#include <regex>

using namespace geode::prelude;

GDPSMain* GDPSMain::m_instance = nullptr;

bool GDPSMain::isActive() const {
    if (!m_issues.empty())
        return false;
    if (isBase())
        return false;
    return true;
}

void GDPSMain::registerIssue(const std::string& issue) {
    m_issues.push_back(issue);
}

std::vector<std::string> GDPSMain::getIssues() {
    return m_issues;
}

void GDPSMain::save() const {
    // Due to Geode wanting to save Server objects themselves and us wanting to store
    // shared pointers, we have to copy into a new map because I can't be fucked to figure
    // out a way that avoids copying right now. TODO: Make ts better later
    std::map<int, GDPSTypes::Server> servers;
    for (const auto& [id, server] : m_servers) {
        if (id == ServerID::RobTop || id == ServerID::None)
            continue;
        servers[id] = *server.get();
    };

    Mod::get()->setSavedValue<std::map<int, GDPSTypes::Server>>("servers", servers);
}

[[nodiscard]] geode::Result<> GDPSMain::setServerSaveDir(std::shared_ptr<GDPSTypes::Server> server, std::string_view saveDir) {
    if (saveDir.empty()) {
        return geode::Err("Cannot make save directory empty.");
    }

    if (server->saveDir == saveDir) {
        log::warn("Attempted to set server save directory to itself.");
        return geode::Ok();
    }

    auto newSaveDirPath = Mod::get()->getSaveDir() / "gdpses" / saveDir;
    auto oldSaveDirPath = Mod::get()->getSaveDir() / "gdpses" / server->saveDir;

    std::error_code err;
    bool newDirExists = std::filesystem::exists(newSaveDirPath, err);
    if (newDirExists) {
        return geode::Err("Save directory {} already exists!", newSaveDirPath);
    }
    if (err) {
        return geode::Err("Error checking exitence of save directory {}: {}", newSaveDirPath, err.message());
    }

    auto gdpsesDir = Mod::get()->getSaveDir() / "gdpses";
    auto canonicalNewSaveDirPath = std::filesystem::canonical(newSaveDirPath, err);
    if (err) {
        return geode::Err("Error getting canonical path for save directory {}: {}", newSaveDirPath, err.message());
    }

    if (!geode::utils::string::pathToString(canonicalNewSaveDirPath).starts_with(geode::utils::string::pathToString(gdpsesDir))
        || std::filesystem::equivalent(newSaveDirPath, gdpsesDir)
    ) {
        return geode::Err("Save directory already exists and cannot be overwritten.");
    }

    std::filesystem::rename(geode::dirs::getSaveDir() / "gdpses" / server->saveDir, saveDir);
    server->saveDir = saveDir;

    return geode::Ok();
}

// I have a nagging feeling in my head making this return a shared_ptr could
// cause a memory leak, this may need to be changed later.
geode::Result<std::shared_ptr<GDPSTypes::Server>> GDPSMain::getCurrentServer() {
    // If this ever happens I will kill myself.
    if (!m_servers.contains(m_currentServer)) {
        return geode::Err("Current server not found");
    }
    return geode::Ok(m_servers[m_currentServer]);
}

geode::Result<std::shared_ptr<GDPSTypes::Server>> GDPSMain::getServer(int id) {
    if (!m_servers.contains(id)) {
        return geode::Err(fmt::format("Server at {} not found", id));
    }
    return geode::Ok(m_servers[id]);
}

geode::Result<> GDPSMain::registerServer(std::shared_ptr<GDPSTypes::Server> server) {
    if (m_servers.contains(server->id)) {
        return geode::Err("Server registery already contains this ID.\nContact developers for help.");
    }

    if (server->saveDir.empty()) {
        server->saveDir = fmt::format("{}", server->id);
    }

    m_servers[server->id] = server;
    ServerInfoManager::get()->fetch(server);
    return geode::Ok();
}

geode::Result<> GDPSMain::modifyRegisteredServer(const GDPSTypes::Server& newServerData) {
    if (!m_servers.contains(newServerData.id)) {
        return geode::Err("Server with this ID does not exist.\nContact developers for help.");
    }
    
    auto newSaveDir = newServerData.saveDir;
    if (newServerData.saveDir.empty()) {
        newSaveDir = fmt::format("{}", newServerData.id);
    }

    if (newSaveDir != m_servers[newServerData.id]->saveDir) {
        auto path = geode::dirs::getSaveDir() / "gdpses" / newSaveDir;
        std::error_code errCode;
        bool exists = std::filesystem::exists(path, errCode);

        if (exists) {
            return geode::Err(fmt::format("Save directory \"{}\" already exists!", path));
        }
        if (errCode) {
	        return geode::Err("Error checking validity of save directory: {}", errCode.message());
        }

	    log::info("{}", geode::dirs::getSaveDir() / "gdpses" / m_servers[newServerData.id]->saveDir);

        std::filesystem::rename(geode::dirs::getSaveDir() / "gdpses" / m_servers[newServerData.id]->saveDir, path, errCode);
	    if (errCode) {
	        return geode::Err("Error moving save directory: {}", errCode.message());
        }
    }

    bool infoLoaded = (newServerData.url == m_servers[newServerData.id]->url);

    auto& server = m_servers[newServerData.id];
    server->name = newServerData.name;
    server->url = newServerData.url;
    server->saveDir = newSaveDir;
    server->infoLoaded = infoLoaded;
    server->modInfo = newServerData.modInfo;
    
    if (m_currentServer == newServerData.id) {
        ServerAPIEvents::updateServer(m_serverApiId, newServerData.url);
        GSGManager::updateFileNames();
    }

    ServerInfoManager::get()->fetch(m_servers[newServerData.id]);
    return geode::Ok();
}

geode::Result<> GDPSMain::deleteServer(std::shared_ptr<GDPSTypes::Server> server) {
    // This should only be hit if some GDPSUtils bs happens.
    if (server->id == m_currentServer) {
        return geode::Err("Attempting to delete server currently in use.");
    }

    bool shouldRestore = m_shouldSaveGameData;
    m_shouldSaveGameData = false;

    std::filesystem::path serverPath = geode::dirs::getSaveDir() / "gdpses" / server->saveDir;
    std::filesystem::path gdpsesDir = geode::dirs::getSaveDir() / "gdpses";
    std::error_code err;

    if (!std::filesystem::exists(serverPath, err) && err) {
        return geode::Err("Failed to check existence of path: {}\n\nnerd shit: std::error_code message: {}", serverPath, err.message());
    }

    err.clear();
    std::filesystem::path canonicalServerPath;
    canonicalServerPath.clear();
    canonicalServerPath = std::filesystem::canonical(serverPath, err);

        if (
            !geode::utils::string::pathToString(canonicalServerPath).starts_with(geode::utils::string::pathToString(gdpsesDir))
            || serverPath == gdpsesDir
        ) {
            return geode::Err(
                "Attempted to delete a path outside or equal to the gdpses directory: {}\n\n"
                "To prevent unintentional extra data loss, your save was not deleted - "
                "only saves within {} will be deleted. If you want to delete this data, do it manually.",
                serverPath, gdpsesDir
            );
        }

        log::debug("Deleting {}", serverPath);
        std::filesystem::remove_all(serverPath, err);
        if (err) {
            return geode::Err("Failed to delete save data for {}: {}", server->name, err.message());
    }

    m_servers.erase(server->id);
    m_shouldSaveGameData = shouldRestore;
    this->save();

    return geode::Ok();
}

geode::Result<> GDPSMain::deleteServer(int id) {
    auto it = m_servers.find(id);
    if (it == m_servers.end()) {
        return geode::Err("Server does not exist!");
    }
    return deleteServer(it->second);
}


GDPSTypes::ServerInvalidity GDPSMain::isValidServer(const GDPSTypes::Server& server) const {
    using namespace GDPSTypes;
    ServerInvalidity validity = ServerInvalidity::Valid;
    // Regex is scary!
    static std::basic_regex urlRegex = std::regex("(http|https):\\/\\/([\\w_-]+(?:(?:\\.[\\w_-]+)+))([\\w.,@?^=%&:\\/~+#-]*[\\w@?^=%&\\/~+#-])");

    if (server.name.empty())
        validity |= ServerInvalidity::NameEmpty;
    if (server.url.empty())
        validity |= ServerInvalidity::UrlEmpty;
    else if (!std::regex_match(server.url, urlRegex))
        validity |= ServerInvalidity::UrlInvalid;

    return validity;
}

bool GDPSMain::serverExists(int id) const {
    return m_servers.find(id) != m_servers.end();
}

void GDPSMain::init() {
    for (const auto& [id, server] : 
        Mod::get()->getSavedValue<std::map<int, GDPSTypes::Server>>("servers")
    ) {
        m_servers[id] = std::make_shared<GDPSTypes::Server>(server);
    }
    m_currentServer =
        Mod::get()->getSavedValue<int>("current", -2);

    auto base = GDPSTypes::Server{-2, "Built-in Servers", ServerAPIEvents::getBaseUrl(), ".."};
    base.iconIsSprite = true;
    base.icon = "gdlogo.png"_spr;
    base.motd = "Vanilla Geometry Dash servers.";
    // ReSharper disable once CppDFAArrayIndexOutOfBounds
    m_servers[-2] = std::make_shared<GDPSTypes::Server>(base);
    auto* server = m_servers[m_currentServer].get();
    if (m_currentServer >= 0 && isActive()) {
        log::info("Loading into GDPS: {}", server->url);
        m_serverApiId = ServerAPIEvents::registerServer(server->url, -40).id;
    }

    m_ServerAPIUpdateListener = ServerUpdatingEvent().listen([this](const Mod* updater, const std::string& url) -> geode::ListenerResult {
        if (url != m_servers[m_currentServer]->url) {
            m_shouldSaveGameData = false;
        } else {
            m_shouldSaveGameData = true;
        }
        return geode::ListenerResult::Propagate;
    });
}

GDPSMain* GDPSMain::get() {
    if (!m_instance) {
        m_instance = new GDPSMain;
        m_instance->init();
    }
    return m_instance;
}

bool GDPSMain::shouldSaveGameData() const {
    return m_shouldSaveGameData;
}

int GDPSMain::currentServer() const {
    return m_currentServer;
}

bool GDPSMain::isBase() const {
    return
        m_currentServer == -2;
}

bool GDPSMain::isBase(std::string url) {
    return 
        url == ServerAPIEvents::getBaseUrl()
    ||  fmt::format("{}/", url) == ServerAPIEvents::getBaseUrl();
}
