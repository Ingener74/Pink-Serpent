#pragma once

namespace cocos2d {
class Node;
}

class Cell {
public:
    Cell(cocos2d::Node*, const std::string& row, const std::string& column);
    virtual ~Cell();

    void setEnable(bool);

    void setPosition(const cocos2d::Vec2&);

private:
    cocos2d::Sprite* m_spriteEnabled = nullptr, *m_spriteDisabled = nullptr;
    cocos2d::Label* m_label = nullptr;
};
