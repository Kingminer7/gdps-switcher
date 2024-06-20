#include <Geode/modify/CCHttpClient.hpp>
#include "../utils/DataManager.hpp"

using namespace geode::prelude;

class $modify(http, CCHttpClient) {
	void send(CCHttpRequest* req)
    {
		std::string url = req->getUrl();
		auto newUrl = DataManager::get().getServer();
		if (newUrl.empty()) {
			newUrl = "https://www.boomlings.com/database";
			Mod::get()->setSavedValue("server", newUrl);
		}
        if (newUrl.ends_with("/")) {
			if (url.starts_with("https://www.boomlings.com/database/")) {
				req->setUrl(url.replace(0, 35, newUrl).c_str());
			} else if (url.starts_with("http://www.boomlings.com/database/")) {
				req->setUrl(url.replace(0, 34, newUrl).c_str());
			}
		} else {
			if (url.starts_with("https://www.boomlings.com/database")) {
				req->setUrl(url.replace(0, 34, newUrl).c_str());
			} else if (url.starts_with("http://www.boomlings.com/database")) {
				req->setUrl(url.replace(0, 33, newUrl).c_str());
			}
		}
        CCHttpClient::send(req);
    }
};
