#include <GDPSUtils.hpp>
#include <ui/ServerSwitchLayer.hpp>

using namespace GDPSUtils;

$on_mod(Loaded) {
  new EventListener<EventFilter<CreateServerEvent>>(+[](CreateServerEvent* e) {
      auto servers  = Mod::get()->getSavedValue<std::vector<ServerSwitchLayer::ServerEntry>>("saved-servers");
      for (const auto& server : servers) {
        if (server.name == e->m_name && server.url == e->m_url) {
          return ListenerResult::Stop;
        }
      }
      servers.push_back({e->m_name, e->m_url});
      Mod::get()->setSavedValue("saved-servers", servers);
      e->m_success = true;
      return ListenerResult::Stop;
  });

  new EventListener<EventFilter<DeleteServerEvent>>(+[](DeleteServerEvent* e) {
      auto servers  = Mod::get()->getSavedValue<std::vector<ServerSwitchLayer::ServerEntry>>("saved-servers");
      for (const auto& server : servers) {
        if (server.name == e->m_name && server.url == e->m_url) {
          return ListenerResult::Stop;
        }
      }
      servers.push_back({e->m_name, e->m_url});
      Mod::get()->setSavedValue("saved-servers", servers);
      e->m_success = true;
      return ListenerResult::Stop;
  });
}