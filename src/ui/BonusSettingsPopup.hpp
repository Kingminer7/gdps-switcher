#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>

class BonusSettingsPopup : public geode::Popup<> {
    protected:
        bool setup() override;
        geode::ScrollLayer *m_scroll = nullptr;
        std::vector<cocos2d::CCMenu*> m_options = {};
        bool m_reloadLayer = false;
    public:
        static BonusSettingsPopup *create();

        void onClose(CCObject *sender) override;
};