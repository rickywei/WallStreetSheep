cmake_minimum_required(VERSION 3.25)

project(WallStreetSheepStrategy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O1 -fPIE -pie -g -fsanitize=thread")
set(CMAKE_BUILD_TYPE Debug)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(
  ${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}
)
aux_source_directory(${PROJECT_SOURCE_DIR} STRATEGY)
add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE_DIR}/factory.cpp
  ${STRATEGY}
)
