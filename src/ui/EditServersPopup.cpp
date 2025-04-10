#include "EditServersPopup.hpp"
#include "Types.hpp"

bool EditServersPopup::setup(std::vector<GDPSTypes::Server> servers) {

    return true;
};

EditServersPopup *EditServersPopup::create(std::vector<GDPSTypes::Server> servers) {
    auto ret = new EditServersPopup();
    if (ret->initAnchored(240.f, 160.f, servers, "geode.loader/GE_square01.png")) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}