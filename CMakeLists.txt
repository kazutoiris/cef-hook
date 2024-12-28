cmake_minimum_required (VERSION 3.10)
project(CefHook)

add_definitions(-DUNICODE -D_UNICODE)
include_directories(${CMAKE_SOURCE_DIR}/cef)
include_directories(${CMAKE_SOURCE_DIR}/chromium)
add_subdirectory(src)
