#include <km7dev.server_api/include/ServerAPI.hpp>

using namespace geode::prelude;

class PSUtils
{
private:
    int serverId;

    PSUtils() {}

    inline static PSUtils *instance = nullptr;

public:
    static PSUtils *get()
    {
        if (!instance) {
            instance = new PSUtils();
            auto url = Mod::get()->getSavedValue<std::string>("server");
            if (url.empty()) {
                url = "https://www.boomlings.com/database/";
                Mod::get()->setSavedValue("server", url);
            }
            instance->initialize(url);
        }
        return instance;
    }

    bool isBoomlings()
    {
        auto server = ServerAPI::get()->getURLById(this->serverId);
        return server.starts_with("www.boomlings.com/database")
        || server.starts_with("http://www.boomlings.com/database")
        || server.starts_with("https://www.boomlings.com/database");
    }

    void initialize(std::string url)
    {
        if (!isBoomlings()) this->serverId = ServerAPI::get()->registerURL(url, -40);
    }

    int getId() {return serverId;}
};