using namespace geode::prelude;

class GDPSManager
{
protected:
    static GDPSManager instance;
    bool needsRestart = false;
    std::string server;

public:
    static GDPSManager get()
    {
        return instance;
    };

    static void init() {
        instance = GDPSManager();
        instance
    };

    void create() {
        server = Mod::get()->getSavedValue<std::string>("server");
    };

}