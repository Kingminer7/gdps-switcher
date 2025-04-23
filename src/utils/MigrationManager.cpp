#include "MigrationManager.hpp"
#include "GDPSMain.hpp"

Result<> MigrationManager::setup() {
    auto savePath = dirs::getSaveDir() / "gdpses";

    if (!std::filesystem::exists(savePath)) {
        std::error_code ec;
        if (!std::filesystem::create_directory(savePath, ec)) {
            return Err(fmt::format("Failed to create directory '{}': {}", savePath.string(), ec.message()));
        }
    } else if (!std::filesystem::is_directory(savePath)) {
        return Err(fmt::format("'{}' exists but is not a directory.", savePath.string()));
    }

    if (Mod::get()->getSavedValue<std::string>("latest") != "1.4.0") {
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

    std::map<int, GDPSTypes::Server> servers;
    for (auto old : Mod::get()->getSavedValue<std::vector<GDPSTypes::OldServer>>("saved-servers")) {
        auto serv = fromOldServer(old);
        servers[serv.id] = serv;
    }

    // Mod::get()->setSavedValue<std::map<int, GDPSTypes::Server>>("servers-v2", servers);

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
            std::error_code ec;
            if (!std::filesystem::exists(dir) && !std::filesystem::create_directory(dir, ec)) {
                log::error("Failed to create directory '{}': {}", dir.string(), ec.message());
                continue;
            }

            auto newName = fname.substr(0, start - 1) + (isBackup ? "2" : "") + fname.substr(end);
            auto newPath = dir / newName;

            std::filesystem::rename(savePath / fname, newPath, ec);
            if (ec) {
                log::error("Failed to move file '{}' to '{}': {}", fname, newPath.string(), ec.message());
            }
        }
    }
    Mod::get()->setSavedValue<std::string>("latest", "1.4.0");
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