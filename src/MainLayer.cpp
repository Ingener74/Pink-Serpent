#include <iostream>
#include "MainLayer.h"
#include "Board.h"

using namespace std;
using namespace cocos2d;

MainLayer::MainLayer() {
}

MainLayer::~MainLayer() {
}

Scene* MainLayer::scene() {
    auto scene = Scene::create();
    auto layer = MainLayer::create();
    scene->addChild(layer);
    return scene;
}

bool MainLayer::init() {
    if (!Layer::init())
        return false;

//    auto mouseListener = EventListenerMouse::create();
//    mouseListener->onMouseDown = [=](EventMouse* event) {
//        cout << __PRETTY_FUNCTION__ << " " << event->getLocation().x << " " << event->getLocation().y << endl;
//    };
//    _eventDispatcher->addEventListenerWithFixedPriority(mouseListener, 1);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    {
        auto closeItem = MenuItemImage::create("button_red_close.png", "button_grey_close.png",
            CC_CALLBACK_1(MainLayer::menuCloseCallback, this));

        closeItem->setPosition(origin + Vec2(visibleSize) - Vec2(closeItem->getContentSize() / 2));

        {
            auto menu = Menu::create(closeItem, nullptr);
            menu->setPosition(Vec2::ZERO);
            addChild(menu, 1);
        }
    }

    {
        auto label = Label::createWithTTF("Pink Serpent", "fonts/arial.ttf", 24.f);
        label->setPosition(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - label->getContentSize().height);
        addChild(label, 1);
    }

    { // Background
        auto background = Sprite::create("background.png");
        auto size = background->getContentSize();
        float scale = visibleSize.width / size.width * 1.f;
        background->setScale(scale);
        background->setPosition(scale * (size.width / 2), scale * (size.height / 2));
        addChild(background);
    }

    m_board.reset(new Board(this));

    return true;
}

void MainLayer::menuCloseCallback(Ref*) {
    Director::getInstance()->end();
}
