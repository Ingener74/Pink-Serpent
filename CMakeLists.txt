cmake_minimum_required(VERSION 3.0)
project(Pink_Serpent)

add_subdirectory(cocos2d)

add_definitions(-std=c++11)
add_definitions(-DLINUX)

include_directories(
        cocos2d/cocos
        cocos2d/external/glfw3/include/mac
        cocos2d/cocos/editor-support
)

set(PinkSerpent_SOURCES
        src/main.cpp
        src/AppDelegate.cpp
        src/MainLayer.cpp
)

add_executable(PinkSerpent ${PinkSerpent_SOURCES})
target_link_libraries(PinkSerpent cocos2d)

add_custom_command(
        TARGET PinkSerpent
        POST_BUILD
        COMMAND cp -R ${CMAKE_SOURCE_DIR}/Resources ${CMAKE_BINARY_DIR}
)