#include "MainLayer.h"

using namespace cocos2d;

Scene* MainLayer::scene()
{
    auto scene = Scene::create();
    
    MainLayer *layer = MainLayer::create();

    scene->addChild(layer);

    return scene;
}

bool MainLayer::init()
{
    if ( !Layer::init() )
        return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto closeItem = MenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        CC_CALLBACK_1(MainLayer::menuCloseCallback,this));
    
    closeItem->setPosition(origin + Vec2(visibleSize) - Vec2(closeItem->getContentSize() / 2));

    auto menu = Menu::create(closeItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    auto label = Label::createWithTTF("Hello World", "fonts/arial.ttf", 24.f);

    label->setPosition(origin.x + visibleSize.width/2, origin.y + visibleSize.height - label->getContentSize().height);

    this->addChild(label, 1);

    auto sprite = Sprite::create("HelloWorld.png");

    sprite->setPosition(Vec2(visibleSize / 2) + origin);

    this->addChild(sprite);
    
    return true;
}

void MainLayer::menuCloseCallback(Ref*)
{
    Director::getInstance()->end();
}
