cmake_minimum_required(VERSION 3.0)
project(Pink_Serpent)

add_definitions(-std=c++11 -DLINUX)

add_subdirectory(cocos2d)

include_directories(
        cocos2d
        cocos2d/cocos
        cocos2d/extensions
        cocos2d/external
        cocos2d/external/glfw3/include/mac
        cocos2d/cocos/editor-support
)

set(PinkSerpent_SOURCES
        src/main.cpp
        src/AppDelegate.cpp
        src/AppDelegate.h # Qt Creator visible
        
        src/MainLayer.cpp
        src/MainLayer.h # Qt Creator visible
)

add_executable(PinkSerpent ${PinkSerpent_SOURCES})

target_link_libraries(PinkSerpent cocos2d)

add_custom_command(
        TARGET PinkSerpent
        POST_BUILD
        COMMAND cp -R ${CMAKE_SOURCE_DIR}/Resources ${CMAKE_BINARY_DIR}
)
