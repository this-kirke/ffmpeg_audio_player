set( audio_player__ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}" )

cmake_minimum_required( VERSION 3.10 )

project( AudioPlayer CXX )
set( CMAKE_VERBOSE_MAKEFILE ON )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

set( THREADS_PREFER_PTHREAD_FLAG ON )
find_package( Threads REQUIRED )

find_package( PkgConfig REQUIRED ) 

#[[
pkg_check_modules(
    AVFORMAT 
    REQUIRED
    IMPORTED_TARGET
    libavformat
)
]]

pkg_check_modules(
    AVDEVICE
    REQUIRED
    IMPORTED_TARGET
    libavdevice
)

add_executable(
    audio_player
    ${audio_player__ROOT_DIR}/src/AudioPlayer.cpp
    ${audio_player__ROOT_DIR}/src/Frame.cpp
    ${audio_player__ROOT_DIR}/src/InputFile.cpp
    ${audio_player__ROOT_DIR}/src/OutputDevice.cpp
    ${audio_player__ROOT_DIR}/src/Packet.cpp
    ${audio_player__ROOT_DIR}/src/Resampler.cpp
    ${audio_player__ROOT_DIR}/src/main.cpp
)

target_include_directories(
    audio_player
    PUBLIC
    ${audio_player__ROOT_DIR}/include
)

target_link_libraries( 
    audio_player
    PRIVATE
    PkgConfig::AVDEVICE
    Threads::Threads
)