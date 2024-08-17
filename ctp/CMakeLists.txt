cmake_minimum_required(VERSION 3.25)

project(WallStreetSheepCtp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -fsanitize=thread")
set(CMAKE_BUILD_TYPE Debug)

find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(ICU COMPONENTS data uc i18n io REQUIRED)
# find_package(concurrentqueue REQUIRED)
find_package(range-v3 REQUIRED)
find_package(redis++ REQUIRED)
find_package(hiredis REQUIRED)

include_directories(
  ${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}
)

aux_source_directory(${PROJECT_SOURCE_DIR} TRADE)
add_executable(${PROJECT_NAME}
  ${TRADE}
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}/ctp_v6.7.2/libthosttraderapi_se.so
  PUBLIC nlohmann_json::nlohmann_json
  PUBLIC spdlog::spdlog
  PUBLIC ICU::data
  PUBLIC ICU::uc
  PUBLIC ICU::i18n
  PUBLIC ICU::io
  # PUBLIC range-v3::range-v3
  PUBLIC redis++::redis++
  PUBLIC hiredis::hiredis
)
