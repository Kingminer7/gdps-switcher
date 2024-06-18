#pragma once
using namespace geode::prelude;

class InstanceManager
{
protected:
    static InstanceManager *instance;
    bool needsRestart = false;
    static bool inited;
    std::string server;

    void init() {
        server = Mod::get()->getSavedValue<std::string>("server");
    };

public:
    static InstanceManager *get()
    {
        return instance;
    };

    static void create() {
        if (inited) return;
        inited = true;
        instance = &InstanceManager();
        instance->init();
    };

    void requestRestart() {
        needsRestart = true;
    };

    std::string getServer() { return server; };

}
