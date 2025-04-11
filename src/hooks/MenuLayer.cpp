#include "ui/ServerListLayer.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace prelude;

class GSMenuLayer : public Modify<GSMenuLayer, MenuLayer> {
    public:
        bool init() {
            if (!MenuLayer::init()) return false;

            if (auto menu = this->getChildByID("bottom-menu")) {
                auto button = CCMenuItemSpriteExtra::create(
                    CircleButtonSprite::createWithSpriteFrameName("switchServer.png"_spr, 1.f, CircleBaseColor::Green, CircleBaseSize::MediumAlt),
                    this,
                    menu_selector(GSMenuLayer::onGDPS)
                );
                menu->addChild(button);
                menu->updateLayout();
            }

            return true;
        }

        void onGDPS(CCObject *sender) {
            auto transition = CCTransitionFade::create(0.5f, ServerListLayer::scene());
            CCDirector::get()->replaceScene(transition);
        }
};