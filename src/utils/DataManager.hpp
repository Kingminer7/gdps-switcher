// I CANNOT mess up save file stuff again. I need to be REALLY REALLY REALLY careful this time.

#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class DataManager {
    protected:
        void migrateData();
    public:
        Result<> setup();
        static std::string urlToFilenameSafe(std::string url);
};