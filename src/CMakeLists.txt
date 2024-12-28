file(GLOB SOURCES "*.cpp" "*.c")
add_library(winmm SHARED ${SOURCES})
set_target_properties(winmm PROPERTIES 
    LINKER_LANGUAGE C
    OUTPUT_NAME "winmm"
    PREFIX ""
)