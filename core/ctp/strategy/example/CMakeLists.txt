cmake_minimum_required(VERSION 3.25)

project(WallStreetSheepCtpStrategyExample LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/ctp/strategy")

find_package(nlohmann_json REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(ICU COMPONENTS data uc i18n io REQUIRED)
find_package(concurrentqueue REQUIRED)
find_package(range-v3 REQUIRED)
find_package(redis++ REQUIRED)
find_package(hiredis REQUIRED)

aux_source_directory(${PROJECT_SOURCE_DIR}/../../../common COMMON)
aux_source_directory(${PROJECT_SOURCE_DIR}/../../encoding ENCODING)
aux_source_directory(${PROJECT_SOURCE_DIR} STRATEGY)
add_executable(${PROJECT_NAME}
  ${STRATEGY}
  ${PROJECT_SOURCE_DIR}/../IStrategy.cpp
  ${ENCODING}
  ${COMMON}
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC nlohmann_json::nlohmann_json
  PUBLIC yaml-cpp::yaml-cpp
  PUBLIC spdlog::spdlog
  PUBLIC ICU::data
  PUBLIC ICU::uc
  PUBLIC ICU::i18n
  PUBLIC ICU::io
  PUBLIC range-v3::range-v3
  PUBLIC redis++::redis++
  PUBLIC hiredis::hiredis
)
