#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

using namespace geode::prelude;

class DataManager
{
private:
    std::string server = "https://www.boomlings.com/database/";

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

    void initialize(std::string url)
    {
        setServer(url);
    }
};