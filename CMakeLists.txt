cmake_minimum_required(VERSION 2.8.3)
project(arbitrage)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-std=c++14 -O2 -Wall -pthread ${CMAKE_CXX_FLAGS}")

set(cpprestsdk_DIR ${PROJECT_SOURCE_DIR}/lib/cpprestsdk)
find_package(cpprestsdk REQUIRED)
find_package(CURL REQUIRED)

include_directories(
    "include"
    ${CURL_INCLUDE_DIRS}
)

set(SOURCES
    src/main.cpp
    src/api.cpp
    # src/exchanges.cpp
    src/binance.cpp
    src/kucoin.cpp
)

add_executable(main ${SOURCES})
target_link_libraries(main
    cpprestsdk::cpprest
    ${CURL_LIBRARIES}
)