class DataManager {
private:
    std::string server;

    DataManager() {
        server = "";
    }

public:
    static DataManager& get() {
        static DataManager instance;
        return instance;
    }

    std::string getServer() const {
        return server;
    }

    void setServer(std::string newServer) {
        server = newServer;
    }

    void initialize(std::string url) {
        setServer(url);
    }
};