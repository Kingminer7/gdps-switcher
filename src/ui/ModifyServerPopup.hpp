/*
 * Named modify instead of edit to avoid confusion
 * cause, well, EditServerPopup and ModifyServerPopup would be confusing
 */

#pragma once

#include "ui/ServerListLayer.hpp"
#include "Types.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ModifyServerPopup : public Popup<GDPSTypes::Server, ServerListLayer *> {
    protected:
        TextInput *m_nameInput = nullptr;
        TextInput *m_urlInput = nullptr;
        ServerListLayer *m_listLayer = nullptr;
        GDPSTypes::Server m_server;

        bool m_isNew = true;

        bool setup(GDPSTypes::Server servers, ServerListLayer * layer) override;
    public:
        virtual void onClose(CCObject *sender) override;
        void onSave(CCObject *sender);
        
        static ModifyServerPopup* create(GDPSTypes::Server servers, ServerListLayer * layer);
};