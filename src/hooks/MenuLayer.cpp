#include "Geode/binding/CCMenuItemSpriteExtra.hpp"
#include "Geode/cocos/CCDirector.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCTransition.h"
#include "Geode/ui/BasedButtonSprite.hpp"
#include "ui/ServerListLayer.hpp"
#include <Geode/modify/MenuLayer.hpp>

class GSMenuLayer : public geode::Modify<GSMenuLayer, MenuLayer> {
    public:
        bool init() {
            if (!MenuLayer::init()) return false;

            if (auto menu = this->getChildByID("bottom-menu")) {
                auto button = CCMenuItemSpriteExtra::create(
                    geode::CircleButtonSprite::createWithSpriteFrameName("switchServer.png"_spr, 1.f, geode::CircleBaseColor::Green, geode::CircleBaseSize::MediumAlt),
                    this,
                    menu_selector(GSMenuLayer::onGDPS)
                );
                menu->addChild(button);
                menu->updateLayout();
            }

            return true;
        }

        void onGDPS(CCObject *sender) {
            auto transition = cocos2d::CCTransitionFade::create(0.5f, ServerListLayer::scene());
            cocos2d::CCDirector::get()->replaceScene(transition);
        }
};