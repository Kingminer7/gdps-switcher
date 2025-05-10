#include "ui/ServerListLayer.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "utils/GDPSMain.hpp"

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

            if (!GDPSMain::get()->getIssues().empty()) {
                queueInMainThread([this](){
                    std::string strungIssues = "";
                    for (auto issue : GDPSMain::get()->getIssues()) {
                        strungIssues += fmt::format("{}\n\n", issue);
                    }
                    auto alert = MDPopup::create(
                        "GDPS Switcher",
                        fmt::format("## Your GDPS is not enabled due to issues. Please fix the following:\n\n{}", strungIssues),
                        "OK"
                    );
                    alert->show();
                    alert->m_scene = getParent();
                });
            }

            return true;
        }

        void onGDPS(CCObject *sender) {
            auto transition = CCTransitionFade::create(0.5f, ServerListLayer::scene());
            CCDirector::get()->pushScene(transition);
        }
};