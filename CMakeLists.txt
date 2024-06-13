cmake_minimum_required(VERSION 3.25)

project(AlphaMaker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

find_package(nlohmann_json REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)
# find_package(fmt REQUIRED)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/market MARKET)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cpp ${MARKET})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/inc)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/api)
target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}/lib/ctp_v6.7.2/libthostmduserapi_se.so
  PUBLIC nlohmann_json::nlohmann_json
  PUBLIC yaml-cpp::yaml-cpp
  PUBLIC spdlog::spdlog
  # PUBLIC fmt::fmt
)
