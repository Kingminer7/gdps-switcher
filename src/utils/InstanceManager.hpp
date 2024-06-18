using namespace geode::prelude;

class InstanceManager
{
protected:
    static InstanceManager instance;
    bool needsRestart = false;
    std::string server;

    void create() {
        server = Mod::get()->getSavedValue<std::string>("server");
    };

public:
    static InstanceManager get()
    {
        return instance;
    };

    static void init() {
        if (instance) return;
        instance = InstanceManager();
    };

    void requestRestart() {
        needsRestart = true;
    };

}
