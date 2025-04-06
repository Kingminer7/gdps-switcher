#ifndef GDPSUTILS_HPP
#define GDPSUTILS_HPP

#ifdef GEODE_IS_WINDOWS
    #ifdef GDPSUTILS_EXPORTS
        #define GDPSUTILS_API __declspec(dllexport)
    #else
        #define GDPSUTILS_API __declspec(dllimport)
    #endif
#else
    #define GDPSUTILS_API
#endif

namespace GDPSUtils {
  struct CreateServerEvent : geode::Event {
    CreateServerEvent(std::string name, std::string url) : m_name(std::move(name)), m_url(std::move(url)) {}

    bool m_success = false;
    std::string m_url = "";
    std::string m_name = "";
  };

  struct DeleteServerEvent : geode::Event {
    DeleteServerEvent(std::string name, std::string url) : m_name(std::move(name)), m_url(std::move(url)) {}

    bool m_success = false;
    std::string m_url = "";
    std::string m_name = "";
  };
  
  inline bool createServer(std::string name, std::string url) {
    auto event = CreateServerEvent(name, url);
    event.post();
    return event.m_success;
  }

  inline bool deleteServer(std::string name, std::string url) {
    auto event = DeleteServerEvent(name, url);
    event.post();
    return event.m_success;
  }
};

#endif