#include <Types.hpp>

using namespace GDPSTypes;

class GDPSMain {
    protected:
        static GDPSMain *m_instance;
        std::vector<std::string> m_issues = {};
        Server m_currentServer;
        void init();
    public:
        bool isActive();
        void registerIssue(std::string issue);
        Server getServer();
        bool isBase();
        bool isBase(std::string url);

        static GDPSMain *get();
};