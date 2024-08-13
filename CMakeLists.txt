cmake_minimum_required(VERSION 3.25)

project(WallStreetSheep LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O1 -fPIE -pie -g -fsanitize=thread")
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(core/ctp/market)
add_subdirectory(core/ctp/trade)
add_subdirectory(core/ctp/strategy/example)
