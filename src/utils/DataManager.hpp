#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class DataManager
{
private:
    std::string server = "https://www.boomlings.com/database/";

    bool serverChecked = false;
    bool serverIntegrated = false;
    bool usingBannedMods = false;
    EventListener<web::WebTask> listener;

    DataManager() {}

    inline static DataManager *instance = nullptr;

    void checkServer()
    {
        if (serverChecked == true) return;
        auto url = server + (server.ends_with("/") ? "" : "/") + "getModMetadata.php";
        web::WebRequest req = web::WebRequest();
        listener.bind([this] (web::WebTask::Event* e) {
            if (web::WebResponse* value = e->getValue()) {
                if (!value->ok()) {
                    if (value->code() >= 400 && value->code() < 500) {
                        serverChecked = true;
                        return;
                    } else {
                        log::error("Failed to check server.");
                        return;
                    }
                }
                auto resp = value->string().unwrapOr("-1");
                if (resp == "-1") {
                    log::error("Failed to check server.");
                    return;
                }
                auto listType = resp.substr(0, resp.find(";"));
                if (listType != "blacklist" && listType != "whitelist") {
                    log::error("Failed to check server.");
                    return;
                }
                auto listStr = resp.substr(resp.find(";") + 1);
                if (listStr.empty()) {
                    log::error("Failed to check server.");
                    return;
                }
               auto listedMods = string::split(listStr, ",");
                
                serverChecked = true;
                serverIntegrated = true;

                auto bannedMods = std::vector<std::string>{};
                
                for (auto mod : listedMods) {
                    if (Loader::get()->isModLoaded(mod)) {
                        usingBannedMods = true;
                        bannedMods.push_back(mod);
                    }
                }

                if (usingBannedMods) {
                    FLAlertLayer::create(
                        "Banned Mods",
                        "You are using mods that are not allowed on this server. Please disable the following mods and try again:\n\n" + string::join(bannedMods, ", "),
                        "Ok"
                    )->show();
                }

            } else if (web::WebProgress* progress = e->getProgress()) {
            } else if (e->isCancelled()) {
            }
        });
        listener.setFilter(req.post(url));
    }

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

    std::string canUseServers() {
        if (!serverChecked) checkServer();
        if (!serverChecked) return "checkFail";
        if (usingBannedMods) return "bannedMods";
        return "success";
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
        checkServer();
    }
};