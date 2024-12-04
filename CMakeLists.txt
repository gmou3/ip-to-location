cmake_minimum_required(VERSION 3.10)
project(ip-to-location)

find_package(CURL REQUIRED)

# deflate
option(LIBDEFLATE_BUILD_SHARED_LIB "Build the shared library" OFF)
option(LIBDEFLATE_COMPRESSION_SUPPORT "Support compression" OFF)
option(LIBDEFLATE_ZLIB_SUPPORT "Support the zlib format" OFF)
add_subdirectory(third-party/libdeflate)

# maxminddb
option(BUILD_TESTING "Build test programs" OFF)
option(MAXMINDDB_BUILD_BINARIES "Build binaries" OFF)
option(MAXMINDDB_INSTALL "Generate the install target" OFF)
add_subdirectory(third-party/libmaxminddb)

# IPToLocation library
add_library(IPToLocation lib/IPToLocation.cc)
target_include_directories(IPToLocation PUBLIC include)
target_link_libraries(IPToLocation curl libdeflate_static maxminddb)

# ip-to-location executable
add_executable(ip-to-location src/ip-to-location.cc)
target_link_libraries(ip-to-location IPToLocation)
