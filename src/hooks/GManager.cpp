#include <Geode/modify/GManager.hpp>
#include "../utils/PSUtils.hpp"
#include <filesystem>
#include <km7dev.server_api/include//ServerAPIEvents.hpp>

using namespace geode::prelude;

class $modify(GDPSGManager, GManager) {
    std::string urlToFilenameSafe(std::string url) {
        std::string safe;
        for (char c : url) {
            if (std::isalnum(c) || c == '.' || c == '-' || c == '_') {
                safe += c;
            } else {
                safe += '_';
            }
        }
        return safe;
    }
    void setup() {
        if (!PSUtils::get()->isBoomlings() && PSUtils::get()->getConflicts().empty()) {
            auto url = PSUtils::get()->getServer().url;
            std::transform(url.begin(), url.end(), url.begin(), ::tolower);
            auto filename = std::string(m_fileName);
            filename.insert(std::string_view(filename).find(".dat"), "-" + urlToFilenameSafe(url));
            if (filename.size() > 5 && std::filesystem::exists(dirs::getSaveDir() / (filename.substr(0, filename.size()-5) + ".dat"))) {
                PSUtils::get()->saveRecovery.push_back(std::pair<std::string, std::string>(url.substr(0, url.size()-1), filename.substr(0, filename.size()-5) + ".dat"));
            }
            m_fileName = filename;
        }
        GManager::setup();
    }

    void save() {
        auto fn = std::string(m_fileName);
        if (fn.size() > 5 && std::find(PSUtils::get()->recovering.begin(), PSUtils::get()->recovering.end(), fn.substr(0, fn.size()-5) + ".dat") != PSUtils::get()->recovering.end()) {
            auto file = dirs::getSaveDir() / (fn.substr(0, fn.size()-5) + ".dat");
            // no crash 4 u
            if (std::filesystem::exists(file))
                std::filesystem::rename(file, dirs::getSaveDir() / fn);

        } else {
            GManager::save();
        }
    }
};
