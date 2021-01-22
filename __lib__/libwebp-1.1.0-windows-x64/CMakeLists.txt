cmake_minimum_required(VERSION 3.17)
project(libwebp)

add_library(libwebp INTERFACE)
target_include_directories(libwebp INTERFACE include)
target_link_libraries(libwebp INTERFACE ${libwebp_SOURCE_DIR}/lib/libwebp.lib)
