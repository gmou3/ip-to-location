cmake_minimum_required(VERSION 3.10)
project(ip-to-location)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(third-party/curl)
add_subdirectory(third-party/libdeflate)
add_subdirectory(third-party/libmaxminddb)

add_library(IPToLocation lib/IPToLocation.cc)
target_include_directories(IPToLocation PUBLIC include)
target_link_libraries(IPToLocation libcurl libdeflate_static maxminddb)
add_executable(ip-to-location src/ip-to-location.cc)
target_link_libraries(ip-to-location IPToLocation)
