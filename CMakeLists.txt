cmake_minimum_required(VERSION 3.13)

project(flobopuyo C CXX)

set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT (CMAKE_BUILD_TYPE OR DEFINED ENV{CMAKE_BUILD_TYPE}))
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "${DEFAULT_BUILD_TYPE}")
endif()

if (("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)

set(C_FILES
    audio.c
    glSDL.c
    menu.c
    menuitems.c
    preferences.c
    PuyoDoomMelt.c
    PuyoVersion.c
    scrollingtext.c
    sofont.c
)

add_executable(flobopuyo
    AnimatedPuyo.cpp
    corona32.cpp
    corona.cpp
    corona_palette.cpp
    GameControls.cpp
    HiScores.cpp
    InputManager.cpp
    IosImgProcess.cpp
    main.cpp
    PuyoAnimations.cpp
    PuyoCommander.cpp
    PuyoGame.cpp
    PuyoIA.cpp
    PuyoLock.cpp
    PuyoStarter.cpp
    PuyoView.cpp
    SDL_Painter.cpp
    ${C_FILES}
)

set_property(TARGET flobopuyo PROPERTY CXX_STANDARD 11)
set_property(TARGET flobopuyo PROPERTY CXX_EXTENSIONS OFF)

target_compile_definitions(flobopuyo
    PRIVATE USE_AUDIO
    PRIVATE HAVE_OPENGL
    PRIVATE DATADIR=\"${CMAKE_SOURCE_DIR}/data\"
)

if(APPLE)
    target_compile_definitions(flobopuyo
        PRIVATE GL_SILENCE_DEPRECATION
    )
endif()

target_include_directories(flobopuyo
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(flobopuyo
    SYSTEM
    PRIVATE ${SDL_INCLUDE_DIRS}
    PRIVATE ${SDL_IMAGE_INCLUDE_DIRS}
    PRIVATE ${SDL_MIXER_INCLUDE_DIRS}
)

target_link_libraries(flobopuyo
    ${OPENGL_LIBRARIES}
    ${SDL_LIBRARIES}
    ${SDL_IMAGE_LIBRARIES}
    ${SDL_MIXER_LIBRARIES}
)
