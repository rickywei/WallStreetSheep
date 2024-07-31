cmake_minimum_required(VERSION 3.25)

project(WallStreetSheep LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

find_package(nlohmann_json REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(libpqxx REQUIRED)
# find_package(fmt REQUIRED)
find_package(ICU COMPONENTS data uc i18n io REQUIRED)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/common COMMON)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/db DB)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/market MARKET)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/trade TRADE)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cpp
  ${COMMON}
  ${DB}
  ${MARKET}
  ${TRADE}
)
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}/inc
  PUBLIC ${PROJECT_SOURCE_DIR}/api
)
target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC ${PROJECT_SOURCE_DIR}/lib/ctp_v6.7.2/libthostmduserapi_se.so
  PUBLIC ${PROJECT_SOURCE_DIR}/lib/ctp_v6.7.2/libthosttraderapi_se.so
  PUBLIC nlohmann_json::nlohmann_json
  PUBLIC yaml-cpp::yaml-cpp
  PUBLIC spdlog::spdlog
  PUBLIC libpqxx::pqxx
  # PUBLIC fmt::fmt
  PUBLIC ICU::data
  PUBLIC ICU::uc
  PUBLIC ICU::i18n
  PUBLIC ICU::io
)
