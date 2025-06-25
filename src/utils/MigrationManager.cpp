#include "MigrationManager.hpp"
#include "GDPSMain.hpp"

#include <km7dev.server_api/include/ServerAPIEvents.hpp>

Result<> MigrationManager::setup() {
    auto savePath = dirs::getSaveDir() / "gdpses";

    std::error_code err;
    if (!std::filesystem::exists(savePath, err)) {
        if (err) {
            return Err(fmt::format("Failed to check existence of '{}': {}", savePath.string(), err.message()));
        }
        if (!std::filesystem::create_directory(savePath, err)) {
            return Err(fmt::format("Failed to create directory '{}': {}", savePath.string(), err.message()));
        }
    } else if (!std::filesystem::is_directory(savePath, err)) {
        if (err) {
            return Err(fmt::format("Failed to check if '{}' is a directory: {}", savePath.string(), err.message()));
        }
        return Err(fmt::format("'{}' exists but is not a directory.", savePath.string()));
    }

    if (Mod::get()->getSavedValue<bool>("1.4.0-migration") != true) {
        migrateData();
    }

    return Ok();
}

std::string MigrationManager::urlToFilenameSafe(const std::string url) {
    std::string ret = "";
    for (char c : url) {
        if (std::isalnum(c) || c == '.' || c == '-' || c == '_') {
            ret += c;
        } else {
            ret += '_';
        }
    }
    return ret;
}

void MigrationManager::migrateData() {
    auto savePath = dirs::getSaveDir();

    auto main = GDPSMain::get();
    auto oldSel = Mod::get()->getSavedValue<std::string>("server", "https://www.boomlings.com/database/");

    for (auto old : Mod::get()->getSavedValue<std::vector<GDPSTypes::OldServer>>("saved-servers")) {
        auto serv = fromOldServer(old);
        main->m_servers[serv.id] = serv;
        if (oldSel == serv.url) {
            main->m_currentServer = serv.id;
            ServerAPIEvents::updateServer(main->m_serverApiId, serv.url);
            Mod::get()->setSavedValue<int>("current", serv.id);
        }
    }

    
    GDPSMain::get()->save();

    auto gdpsPath = savePath / "gdpses";

    for (const auto& entry : std::filesystem::directory_iterator(savePath)) {
        if (!entry.is_regular_file()) continue;

        auto fname = entry.path().filename().string();
        if ((fname.starts_with("CCGameManager-") || fname.starts_with("CCLocalLevels-")) && fname.ends_with(".dat")) {
            auto start = fname.find('-') + 1;
            auto end = fname.rfind(".dat");
            if (start == std::string::npos || end == std::string::npos || start >= end) continue;

            auto name = fname.substr(start, end - start);
            auto isBackup = name.ends_with("2");
            if (isBackup) {
                name = name.substr(0, name.size() - 1);
            }

            auto dir = gdpsPath / name;
            std::error_code err;
            if (!std::filesystem::exists(dir, err)) {
                if (err) {
                    log::error("Failed to check existence of directory '{}': {}", dir.string(), err.message());
                    continue;
                }
                if (!std::filesystem::create_directory(dir, err)) {
                    log::error("Failed to create directory '{}': {}", dir.string(), err.message());
                    continue;
                }
            } else if (!std::filesystem::is_directory(dir, err)) {
                if (err) {
                    log::error("Failed to check if '{}' is a directory: {}", dir.string(), err.message());
                } else {
                    log::error("'{}' exists but is not a directory.", dir.string());
                }
                continue;
            }

            auto newName = fname.substr(0, start - 1) + (isBackup ? "2" : "") + fname.substr(end);
            auto newPath = dir / newName;

            std::filesystem::rename(savePath / fname, newPath, err);
            if (err) {
                log::error("Failed to move file '{}' to '{}': {}", fname, newPath.string(), err.message());
            }
        }
    }
    Mod::get()->setSavedValue<bool>("1.4.0-migration", true);
}

GDPSTypes::Server MigrationManager::fromOldServer(GDPSTypes::OldServer server) {
    GDPSTypes::Server ret = GDPSTypes::Server();
    int i = 0;
    for (auto &[id, server] : GDPSMain::get()->m_servers) {
        if (id == i) {
            i++;
        }
    }
    ret.id = i;
    ret.name = server.name;
    ret.url = server.url;
    ret.saveDir = urlToFilenameSafe(server.url);
    return ret;
}

$on_mod(Loaded) {
    auto sm = std::make_unique<MigrationManager>();
    auto a = sm->setup();
}