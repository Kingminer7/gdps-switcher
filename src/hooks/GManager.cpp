#include <Geode/modify/GManager.hpp>
#include "../utils/PSUtils.hpp"
#include <km7dev.server_api/include/ServerAPI.hpp>

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
        if (!PSUtils::get()->isBoomlings()) {
            auto url = ServerAPI::get()->getURLById(PSUtils::get()->getId());
            std::transform(url.begin(), url.end(), url.begin(), ::tolower);
            auto filename = std::string(m_fileName);
            filename.insert(std::string_view(filename).find(".dat"), "-" + urlToFilenameSafe(url));
            m_fileName = filename;
        }
        GManager::setup();
    }
};
