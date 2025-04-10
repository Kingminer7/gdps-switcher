#include <Types.hpp>

using namespace GDPSTypes;

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

        std::vector<Server> m_servers;
        Server m_currentServer;
};