cmake_minimum_required(VERSION 2.8.3)
project(arbitrage)

add_definitions(-std=c++11)

find_package(CURL REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})

include_directories(
    ${CURL_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
)

# add_executable(main main.cpp)
# target_link_libraries(main
#     ${CURL_LIBRARIES}
#     ${JSONCPP_LIBRARIES}
# )

add_executable(main src/main.cpp src/api.cpp)
target_link_libraries(main
    ${CURL_LIBRARIES}
    ${JSONCPP_LIBRARIES}
)