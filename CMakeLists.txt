cmake_minimum_required(VERSION 3.0)
project(Pink_Serpent)

add_subdirectory(cocos2d)

add_definitions(-std=c++11 -fexceptions)

if(MSVC)
	add_definitions(-DWIN32)
else()
	add_definitions(-DLINUX)
endif()

include_directories(
        cocos2d
        cocos2d/cocos
        cocos2d/extensions
        cocos2d/external
        cocos2d/external/glfw3/include/mac
        cocos2d/cocos/editor-support
)

if(MSVC)
	include_directories(cocos2d/external/win32-specific/gles/include/OGLES)
endif()

set(PinkSerpent_SOURCES
        src/main.cpp
        src/AppDelegate.cpp
        src/AppDelegate.h # Qt Creator visible
        
        src/MainLayer.cpp
        src/MainLayer.h # Qt Creator visible
)

add_executable(PinkSerpent ${PinkSerpent_SOURCES})

if(MSVC)
	link_directories(
		cocos2d/external/win32-specific/gles/prebuilt
	)
	target_link_libraries(PinkSerpent cocos2d glew32)
else()
	target_link_libraries(PinkSerpent cocos2d)
endif()

if(MSVC)
else()
	add_custom_command(
	        TARGET PinkSerpent
	        POST_BUILD
	        COMMAND cp -R ${CMAKE_SOURCE_DIR}/Resources ${CMAKE_BINARY_DIR}
	)
endif()
