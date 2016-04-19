
#include <iostream>

#include "AppDelegate.h"

using namespace std;
using namespace cocos2d;

int main(int argc, char* argv[]){
    try{
        AppDelegate app;
        return Application::getInstance()->run();
   } catch (const std::exception& e){
        cerr << e.what() << endl;
    }
}