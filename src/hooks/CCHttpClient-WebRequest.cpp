#include <Geode/modify/CCHttpClient.hpp>
#include <Geode/utils/web.hpp>
#include "../utils/DataManager.hpp"

using namespace geode::prelude;

class $modify(CCHttpClient) {
	void send(CCHttpRequest* req)
    {
		std::string url = req->getUrl();
		auto newUrl = DataManager::get()->getServer();
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

web::WebTask webSend(web::WebRequest *self, std::string_view method, std::string_view givenUrl)
    {
		auto url = std::string(givenUrl);
		auto newUrl = DataManager::get()->getServer();
		if (newUrl.ends_with("/")) {
			if (url.starts_with("https://www.boomlings.com/database/")) {
				url = url.replace(0, 35, newUrl);
			} else if (url.starts_with("http://www.boomlings.com/database/")) {
				url = url.replace(0, 34, newUrl);
			}
		} else {
			if (url.starts_with("https://www.boomlings.com/database")) {
				url = url.replace(0, 34, newUrl);
			} else if (url.starts_with("http://www.boomlings.com/database")) {
				url = url.replace(0, 33, newUrl);
			}
		}
		return self->send(method, std::string_view(url));
    };

$execute
{
    auto hook = Mod::get()->hook(
        reinterpret_cast<void*>(
			geode::addresser::getNonVirtual(&web::WebRequest::send)
        ),
        &webSend,
        "web::WebRequest::send",
        tulip::hook::TulipConvention::Thiscall
    );
}