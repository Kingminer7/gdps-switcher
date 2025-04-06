#include <Geode/modify/GManager.hpp>
#include "../utils/GDPSMain.hpp"
#include "../utils/DataManager.hpp"
#include "Geode/loader/Log.hpp"

class GSGManager : public geode::Modify<GSGManager, GManager> {
    public:
        void setup() {
            geode::log::info("{}", m_fileName);
            auto gdpsm = GDPSMain::get();
            if (gdpsm->isActive()) {
                m_fileName = fmt::format("gdpses/{}/{}", DataManager::urlToFilenameSafe(gdpsm->getServer().url), m_fileName);
                geode::log::info("Updated file name: {}", m_fileName);
            }
            GManager::setup();
        }
};