#include <assert.h>
#include <cocos2d.h>
#include "Cell.h"

using namespace std;
using namespace cocos2d;

Cell::Cell(Node* layer, const std::string& row, const std::string& column) {
    assert(layer);
    assert(!row.empty());
    assert(!column.empty());
    {
        m_spriteEnabled = Sprite::create("cell_normal/" + row + column + ".png");
        assert(m_spriteEnabled);
        m_spriteEnabled->retain();
        m_spriteEnabled->setVisible(true);
        layer->addChild(m_spriteEnabled);
    }
    {
        m_spriteDisabled = Sprite::create("cell_disabled/" + row + column + ".png");
        assert(m_spriteDisabled);
        m_spriteDisabled->retain();
        m_spriteDisabled->setVisible(false);
        layer->addChild(m_spriteDisabled);
    }
    {
        m_label = Label::createWithTTF(row + column, "fonts/arial.ttf", 12);
        assert(m_label);
        m_label->retain();
        m_label->setColor(Color3B{0, 0, 0});
        layer->addChild(m_label);
    }
}

Cell::~Cell() {
    assert(m_spriteEnabled);
    assert(m_spriteDisabled);
    assert(m_label);
    m_spriteEnabled->release();
    m_spriteDisabled->release();
    m_label->release();
}

void Cell::setEnable(bool enable) {
    assert(m_spriteEnabled);
    assert(m_spriteDisabled);
    m_spriteEnabled->setVisible(enable);
    m_spriteDisabled->setVisible(!enable);
}

void Cell::setPosition(const cocos2d::Vec2& pos) {
    assert(m_spriteEnabled);
    assert(m_spriteDisabled);
    assert(m_label);
    m_spriteEnabled->setPosition(pos);
    m_spriteDisabled->setPosition(pos);
    m_label->setPosition(pos);
}
