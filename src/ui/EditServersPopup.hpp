#pragma once

#include "Types.hpp"

class EditServersPopup : public geode::Popup<std::vector<GDPSTypes::Server>> {
    protected:
    bool setup(std::vector<GDPSTypes::Server> servers) override;

public:
    static EditServersPopup* create(std::vector<GDPSTypes::Server> servers);
};