#pragma once

#include <Geode/cocos/label_nodes/CCLabelBMFont.h>

class ColorLabel : public cocos2d::CCNode {
    protected:
        int m_charLimit = 0;
        // Rows     Colors      The actual label
        std::vector<std::vector<cocos2d::CCLabelBMFont*>> m_labels = {};
        std::string m_text = "";
        float m_textScale;
        geode::TextRenderer *m_renderer = nullptr;
        void updateLabel();

        ~ColorLabel();

        bool init(std::string text, cocos2d::CCSize size, float scale, int charLimit);
    public:
        static std::vector<std::pair<cocos2d::ccColor3B, std::string>> getColoredText(std::string text, int charLimit = INT_MAX);
        void setText(std::string text);
        std::string getText();
        // Rows     Colors      The actual label

        static ColorLabel* create(std::string text, cocos2d::CCSize size, float scale, int charLimit = INT_MAX);
};