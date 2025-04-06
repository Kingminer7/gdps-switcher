// I CANNOT mess up save file stuff again. I need to be REALLY REALLY REALLY careful this time.

#pragma once

#include <Geode/Result.hpp>
class DataManager {
    protected:
        void migrateData();
    public:
        geode::Result<> setup();
        static std::string urlToFilenameSafe(std::string url);
};