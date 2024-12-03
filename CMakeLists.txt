cmake_minimum_required(VERSION 3.10)
project(ip-to-location)

find_package(CURL REQUIRED)
add_subdirectory(third-party/libdeflate)
add_subdirectory(third-party/libmaxminddb)

add_library(IPToLocation lib/IPToLocation.cc)
target_include_directories(IPToLocation PUBLIC include)
target_link_libraries(IPToLocation curl libdeflate_shared maxminddb)
add_executable(ip-to-location src/ip-to-location.cc)
target_link_libraries(ip-to-location IPToLocation)
