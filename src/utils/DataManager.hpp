#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class DataManager
{
private:
    std::string server = "https://www.boomlings.com/database/";
    std::string globedServer = "https://www.boomlings.com/database/";

    bool hasCheckedServer = false;
    bool serverHasIntegration;
    bool usingBannedMods = false;

    EventListener<web::WebTask> m_listener;

    DataManager() {}

public:
    static DataManager &get()
    {
        static DataManager instance;
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

    void setServer(std::string newServer)
    {
        server = newServer;
    }

    void checkServer() {
        auto req = web::WebRequest();
        m_listener.bind([this](web::WebTask::Event *e)
                        {
            if (web::WebResponse* res = e->getValue()) {
                if (!res->ok()) {
                    if (res->code() >= 400 && res->code() < 500) {
                        hasCheckedServer = true;
                        serverHasIntegration = false;
                    }
                } else {
                    hasCheckedServer = true;
                    serverHasIntegration = true;
                    auto data = res->string().unwrapOr("blacklist;No.Mods"); // The No.Mods is a just in case
                    // type;mod,mod,mod...
                    auto listType = data.substr(0, data.find(';'));
                    auto mods = data.substr(data.find(';') + 1);

                    size_t pos = 0;
                    while ((pos = mods.find(",")) != std::string::npos) {
                        auto modid = mods.substr(0, pos);
                        log::info("{}", modid);
                        mods.erase(0, pos + 1);
                    }
                }
            } else if (web::WebProgress* p = e->getProgress()) {
                log::info("progress: {}", p->downloadProgress().value_or(0.f));
            } else if (e->isCancelled()) {
                log::info("The request was cancelled... So sad :(");
            } });
        m_listener.setFilter(req.post(server + (server.ends_with("/") ? "" : "/") + "getModMetadata.php"));
    }

    void initialize(std::string url)
    {
        setServer(url);
    }
};