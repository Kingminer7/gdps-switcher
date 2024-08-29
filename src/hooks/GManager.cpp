#include <Geode/modify/GManager.hpp>
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
        auto url = ServerAPI::get()->getServerURL();
        std::transform(url.begin(), url.end(), url.begin(), ::tolower);
        if (!(
            url == "https://www.boomlings.com/database/" |
            url == "https://www.boomlings.com/database/"
            )) {
            auto filename = std::string(m_fileName);
            filename.insert(std::string_view(filename).find(".dat"), "-" + urlToFilenameSafe(url));
            m_fileName = filename;
        }
        GManager::setup();
    }
};
