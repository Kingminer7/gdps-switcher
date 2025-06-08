#include "ServerInfoManager.hpp"

using namespace geode::prelude;

ListenerResult LoadDataNodeFilter::handle(std::function<Callback> fn, LoadDataEvent event) {
	if (m_target != event.getServer()) {
		return ListenerResult::Propagate;
	}

    return fn(event);
}

void ServerInfoManager::fetch(GDPSTypes::Server& server) {
	LoadDataEvent(server, "Motd", "https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcSBgNT8zJosfwEyrPM2N4F8pRGfBG80hXwvbuN7cqo9zxb600Jblfujv4E&usqp=CAE&s").post();
}
