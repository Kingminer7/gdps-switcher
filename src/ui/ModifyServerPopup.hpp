/*
 * Named modify instead of edit to avoid confusion
 * cause, well, EditServerPopup and ModifyServerPopup would be confusing
 */

#pragma once

#include "Geode/ui/TextInput.hpp"
#include "Types.hpp"
#include "ui/ServerListLayer.hpp"

class ModifyServerPopup : public geode::Popup<GDPSTypes::Server, ServerListLayer *> {
    protected:
        geode::TextInput *m_nameInput = nullptr;
        geode::TextInput *m_urlInput = nullptr;
        ServerListLayer *m_listLayer = nullptr;
        GDPSTypes::Server m_server;

        bool m_isNew = true;

        bool setup(GDPSTypes::Server servers, ServerListLayer * layer) override;
    public:
        virtual void onClose(CCObject *sender) override;
        void onSave(CCObject *sender);
        
        static ModifyServerPopup* create(GDPSTypes::Server servers, ServerListLayer * layer);
};