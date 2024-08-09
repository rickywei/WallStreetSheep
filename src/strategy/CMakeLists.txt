cmake_minimum_required(VERSION 3.25)

project(WallStreetSheepStrategy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O1 -fPIE -pie -g -fsanitize=thread")
set(CMAKE_BUILD_TYPE Debug)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_compile_options(-fPIC)

# find_package(yaml-cpp REQUIRED)
# find_package(spdlog REQUIRED)
include_directories(
  ${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}
)

aux_source_directory(${PROJECT_SOURCE_DIR}/../common COMMON)
aux_source_directory(${PROJECT_SOURCE_DIR}/../context CONTEXT)
aux_source_directory(${PROJECT_SOURCE_DIR} STRATEGY)

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE_DIR}/factory.cpp
  ${COMMON}
  ${CONTEXT}
  ${STRATEGY}
)

# target_link_libraries(
# ${PROJECT_NAME}
# PUBLIC yaml-cpp::yaml-cpp
# # PUBLIC spdlog::spdlog

# # PUBLIC fmt::fmt
# PUBLIC ICU::data
# PUBLIC ICU::uc
# PUBLIC ICU::i18n
# PUBLIC ICU::io
# )
