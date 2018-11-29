cmake_minimum_required(VERSION 2.8.3)
project(arbitrage)

# add_definitions(-std=c++11)
SET(CMAKE_CXX_FLAGS "-std=c++14 -O2 -Wall -pthread ${CMAKE_CXX_FLAGS}")

find_package(CURL REQUIRED)

include_directories(
    "include"
    ${CURL_INCLUDE_DIRS}
)

set(SOURCES
    src/main.cpp
    src/api.cpp
    src/exchanges.cpp
    src/binance.cpp
    src/kucoin.cpp
)

add_executable(main ${SOURCES})
target_link_libraries(main
    ${CURL_LIBRARIES}
)