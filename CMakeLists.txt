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

find_package(SDL2 REQUIRED)
find_package(SDL2_image QUIET)
find_package(SDL2_mixer QUIET)

if(NOT SDL2_image_FOUND OR NOT SDL2_mixer_FOUND)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND AND NOT SDL2_image_FOUND)
        pkg_check_modules(SDL2_image IMPORTED_TARGET SDL2_image)
    endif()
    if(PkgConfig_FOUND AND NOT SDL2_mixer_FOUND)
        pkg_check_modules(SDL2_mixer IMPORTED_TARGET SDL2_mixer)
    endif()
endif()

if(NOT SDL2_image_FOUND)
    message(FATAL_ERROR "SDL2_image not found")
endif()

if(NOT SDL2_mixer_FOUND)
    message(FATAL_ERROR "SDL2_mixer not found")
endif()

if(APPLE)
    find_library(FRAMEWORK_FOUNDATION Foundation)
endif()

set(C_FILES
    audio.c
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
    PRIVATE DATADIR=\"${CMAKE_SOURCE_DIR}/data\"
)

target_include_directories(flobopuyo
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(flobopuyo
    SYSTEM
    PRIVATE ${SDL2_INCLUDE_DIRS}
    PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS}
    PRIVATE ${SDL2_MIXER_INCLUDE_DIRS}
)

target_link_libraries(flobopuyo PRIVATE
    $<$<BOOL:${MINGW}>:mingw32>
    $<$<BOOL:${APPLE}>:${FRAMEWORK_FOUNDATION}>
    SDL2::SDL2main
    SDL2::SDL2
    $<IF:$<BOOL:${PkgConfig_FOUND}>,$<TARGET_NAME_IF_EXISTS:PkgConfig::SDL2_image>, $<TARGET_NAME_IF_EXISTS:SDL2_image::SDL2_image>>
    $<IF:$<BOOL:${PkgConfig_FOUND}>,$<TARGET_NAME_IF_EXISTS:PkgConfig::SDL2_mixer>, $<TARGET_NAME_IF_EXISTS:SDL2_mixer::SDL2_mixer>>
)
