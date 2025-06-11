#include "ColorLabel.hpp"

bool ColorLabel::init(std::string text, cocos2d::CCSize size, float scale, int charLimit) {
    m_text = text;
    setContentSize(size);
    m_textScale = scale;
    m_renderer = geode::TextRenderer::create();
    m_renderer->retain();
    m_charLimit = charLimit;

    updateLabel();

    return true;
}

void ColorLabel::updateLabel() {
    auto pos = getPosition();
    auto size = getContentSize();
    removeAllChildren();

    m_renderer->begin(this, {0, 0}, m_obContentSize);
    m_renderer->pushBMFont("chatFont.fnt");
    m_renderer->pushScale(m_textScale);
    m_renderer->pushVerticalAlign(geode::TextAlignment::End);
    m_renderer->pushHorizontalAlign(geode::TextAlignment::Begin);
    auto colored = getColoredText(m_text, m_charLimit);
    for (auto [color, text] : colored) {
        geode::log::info("{}: {}", text, color);
        m_renderer->pushColor(color);
        m_renderer->renderString(text);
    }
    m_renderer->end();
    setPosition(pos);
    setContentSize(size);
}

std::vector<std::pair<cocos2d::ccColor3B, std::string>> ColorLabel::getColoredText(std::string text, int charLimit) {
    static const std::unordered_map<char, std::string> colorMap = {
        {'b', "4A52E1"}, {'g', "40E348"}, {'l', "60ABEF"}, {'j', "32C8FF"},
        {'y', "FFFF00"}, {'o', "FF5A4B"}, {'r', "FF5A5A"}, {'p', "FF00FF"},
        {'a', "9632FF"}, {'d', "FF96FF"}, {'c', "FFFF96"}, {'f', "96FFFF"},
        {'s', "FFDC41"}
    };

    std::vector<std::pair<cocos2d::ccColor3B, std::string>> result;
    size_t pos = 0, last = 0;
    cocos2d::ccColor3B currentColor = cocos2d::ccWHITE;
    int visibleCharCount = 0;

    #define ADD_SEGMENT(segment) \
        if (!(segment).empty()) { \
            if (!result.empty() && result.back().first == currentColor) { \
                result.back().second += (segment); \
            } else { \
                result.emplace_back(currentColor, (segment)); \
            } \
        }

    while ((pos = text.find('<', last)) != std::string::npos) {
        std::string segment = text.substr(last, pos - last);
        for (char c : segment) {
            if (visibleCharCount >= charLimit) break;
            ADD_SEGMENT(std::string(1, c));
            visibleCharCount++;
        }
        if (visibleCharCount >= charLimit) break;

        if (pos + 2 < text.size() && text.substr(pos + 1, 2) == "c>") {
            currentColor = cocos2d::ccRED;
            last = pos + 3;
        } else if (pos + 3 < text.size() && text[pos + 1] == 'c' && text[pos + 3] == '>') {
            auto it = colorMap.find(text[pos + 2]);
            if (it != colorMap.end()) {
                auto color = geode::cocos::cc3bFromHexString(it->second);
                currentColor = color.isOk() ? color.unwrap() : currentColor;
            }
            last = pos + 4;
        } else if (pos + 9 < text.size() && text.substr(pos + 1, 2) == "c-" && text[pos + 9] == '>') {
            auto color = geode::cocos::cc3bFromHexString(text.substr(pos + 3, 6));
            if (color.isOk()) {
                currentColor = color.unwrap();
                last = pos + 10;
            } else {
                ADD_SEGMENT(text.substr(pos, 10));
                last = pos + 10;
            }
        } else if (pos + 3 < text.size() && text.substr(pos + 1, 3) == "/c>") {
            currentColor = cocos2d::ccWHITE;
            last = pos + 4;
        } else {
            ADD_SEGMENT(text.substr(pos, 1));
            last = pos + 1;
        }
    }

    if (visibleCharCount < charLimit) {
        std::string segment = text.substr(last);
        for (char c : segment) {
            if (visibleCharCount >= charLimit) break;
            ADD_SEGMENT(std::string(1, c));
            visibleCharCount++;
        }
    }

    return result;
}

void ColorLabel::setText(std::string text) {
    m_text = text;
    updateLabel();
}

std::string ColorLabel::getText() {
    return m_text;
}

ColorLabel* ColorLabel::create(std::string text, cocos2d::CCSize size, float scale, int charLimit) {
    auto label = new ColorLabel();
    if (label && label->init(text, size, scale, charLimit)) {
        label->autorelease();
        return label;
    }
    delete label;
    return nullptr;
}

ColorLabel::~ColorLabel() {
    m_renderer->release();
}