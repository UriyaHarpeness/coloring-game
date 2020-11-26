cmake_minimum_required(VERSION 3.10.2)
project(coloring-game)

set(CMAKE_CXX_STANDARD 17)

add_executable(coloring-game main.cpp
        board/Board.cpp
        board/Board.h
        game/Game.cpp
        game/Game.h)