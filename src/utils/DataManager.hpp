#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class DataManager
{
private:
    std::string server = "https://www.boomlings.com/database/";

    DataManager() {}

    inline static DataManager *instance = nullptr;

public:
    static DataManager *get()
    {
        if (!instance) {
            instance = new DataManager();
            auto url = Mod::get()->getSavedValue<std::string>("server");
            if (url.empty()) {
                url = "https://www.boomlings.com/database";
                Mod::get()->setSavedValue("server", url);
            }
            instance->initialize(url);
        }
        return instance;
    }

    bool isBoomlings()
    {
        return server.starts_with("www.boomlings.com/database") || server.starts_with("http://www.boomlings.com/database") || server.starts_with("https://www.boomlings.com/database");
    }

    std::string getServer() const
    {
        return server;
    }

    void initialize(std::string url)
    {
        server = url;
    }
};