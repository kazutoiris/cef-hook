find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)

file(GLOB SOURCES "*.cpp" "*.c")
add_library(CefHook SHARED ${SOURCES})
set_target_properties(CefHook PROPERTIES 
    LINKER_LANGUAGE C
    OUTPUT_NAME "winmm"
    PREFIX ""
)
target_include_directories(CefHook PRIVATE ${DETOURS_INCLUDE_DIRS})
target_link_libraries(CefHook PRIVATE ${DETOURS_LIBRARY})
