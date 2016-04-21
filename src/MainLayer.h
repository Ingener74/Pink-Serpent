#pragma once

#include <memory>

#include <cocos2d.h>
#include <cocos-ext.h>

class Board;

class MainLayer : public cocos2d::Layer
{
public:
    MainLayer();
    virtual ~MainLayer();

    virtual bool init() override;

    static cocos2d::Scene* scene();

    void menuCloseCallback(Ref* sender);

    CREATE_FUNC(MainLayer);

    void startButtonHandler(Ref*, cocos2d::extension::Control::EventType);

private:
    std::unique_ptr<Board> m_board;
};
