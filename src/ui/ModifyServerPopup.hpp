/*
 * Named modify instead of edit to avoid confusion
 * cause, well, EditServerPopup and ModifyServerPopup would be confusing
 */

#pragma once

#include "ServerListLayer.hpp"
#include "EditServersPopup.hpp"
#include "Types.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ModifyServerPopup : public Popup<GDPSTypes::Server> {
    protected:
        TextInput *m_nameInput = nullptr;
        TextInput *m_urlInput = nullptr;
        ServerListLayer *m_listLayer = nullptr;
        EditServersPopup *m_listPopup = nullptr;
        GDPSTypes::Server m_server;
        GDPSTypes::Server m_orig;

        bool m_isNew = true;

        bool setup(GDPSTypes::Server server) override;
    public:
        virtual void onClose(CCObject *sender) override;
        void onSave(CCObject *sender);
        
        static ModifyServerPopup* create(GDPSTypes::Server server, ServerListLayer *layer);
        static ModifyServerPopup* create(GDPSTypes::Server server, EditServersPopup *popup);
};