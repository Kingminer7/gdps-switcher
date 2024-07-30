#include <Geode/modify/GManager.hpp>
#include "../utils/DataManager.hpp"

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
        auto url = DataManager::get()->getServer();
        std::transform(url.begin(), url.end(), url.begin(), ::tolower);
        if (!DataManager::get()->isBoomlings()) {
            auto filename = std::string(m_fileName);
            filename.insert(std::string_view(filename).find(".dat"), "-" + urlToFilenameSafe(url));
            m_fileName = filename;
        }
        GManager::setup();
    }
};
