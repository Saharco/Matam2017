cmake_minimum_required(VERSION 3.6)
project(MATAM4)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES  EscapeRoom.h EscapeRoom.c Enigma.h EscapeRoomWrapper.h
        Exceptions.h EscapeRoomWrapper.cpp Enigma.cpp EscapeTechnionWrapper_test.cpp)
add_executable(MATAM4 ${SOURCE_FILES})