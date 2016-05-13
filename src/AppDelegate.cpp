#include "AppDelegate.h"

#include "MainLayer.h"

using namespace std;
using namespace cocos2d;

AppDelegate::AppDelegate() {
}

AppDelegate::~AppDelegate() {
}

void AppDelegate::initGLContextAttrs() {
    GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
        glview = GLViewImpl::create("Ping Serpent");
        director->setOpenGLView(glview);
    }

    director->setOpenGLView(glview);

//    Size frameSize = glview->getFrameSize();
//    glview->setFrameSize(1024, 768);

    glview->setDesignResolutionSize(1024, 768, ResolutionPolicy::SHOW_ALL);

    vector<string> searchPath;

    searchPath.push_back("main");

    director->setContentScaleFactor(1.f);

    FileUtils::getInstance()->setSearchPaths(searchPath);

    director->setDisplayStats(false);

    director->setAnimationInterval(1.0 / 60);

    auto scene = MainLayer::scene();

    director->runWithScene(scene);

    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
}
