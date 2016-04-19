#pragma once

#include "cocos2d.h"

class MainLayer : public cocos2d::Layer
{
public:
    virtual bool init() override;

    static cocos2d::Scene* scene();

    void menuCloseCallback(Ref* sender);

    CREATE_FUNC(MainLayer);
};
