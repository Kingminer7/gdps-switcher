// I CANNOT mess up save file stuff again. I need to be REALLY REALLY REALLY careful this time.

#pragma once

#include "Types.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class MigrationManager {
    protected:
        void migrateData();
        GDPSTypes::Server fromOldServer(GDPSTypes::OldServer server);
    public:
        Result<> setup();
        static std::string urlToFilenameSafe(std::string url);
};