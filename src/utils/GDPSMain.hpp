#include "Types.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class GDPSMain {
    protected:
        static GDPSMain *m_instance;
        std::vector<std::string> m_issues = {};
        void init();
    public:
        bool isActive();
        void registerIssue(std::string issue);
        bool isBase();
        bool isBase(std::string url);

        static GDPSMain *get();

        std::map<int, GDPSTypes::Server> m_servers;
        int m_currentServer;
};