set(SOURCES "")

macro(add_dir DIRNAME)
    FILE(GLOB_RECURSE ${DIRNAME}_SOURCES "${DIRNAME}/*.cpp")
    FILE(GLOB_RECURSE ${DIRNAME}_HEADERS "${CMAKE_SOURCE_DIR}/include/atlas/${DIRNAME}/*.hpp")
    source_group(${DIRNAME} FILES ${${DIRNAME}_SOURCES} ${${DIRNAME}_HEADERS})
    set(SOURCES ${SOURCES} ${${DIRNAME}_SOURCES} ${${DIRNAME}_HEADERS})
endmacro()

add_dir(core)
add_dir(util)
add_dir(graphics)

add_library(atlas SHARED ${SOURCES})

install(TARGETS atlas
        RUNTIME DESTINATION bin)

install(TARGETS atlas
        RUNTIME DESTINATION test)
