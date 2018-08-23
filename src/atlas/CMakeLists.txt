set(SOURCES "")

macro(add_dir DIRNAME)
    FILE(GLOB ${DIRNAME}_SOURCES "${DIRNAME}/*.cpp")
    FILE(GLOB ${DIRNAME}_HEADERS "${DIRNAME}/*.hpp")
    source_group(${DIRNAME} FILES ${${DIRNAME}_SOURCES} ${${DIRNAME}_HEADERS})
    set(SOURCES ${SOURCES} ${${DIRNAME}_SOURCES} ${${DIRNAME}_HEADERS})
    install(FILES ${${DIRNAME}_HEADERS}
        	DESTINATION include/atlas/${DIRNAME})
endmacro()

add_dir(core)
add_dir(util)
add_dir(graphics)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(atlas SHARED ${SOURCES})
target_link_libraries(atlas ${Vulkan_LIBRARY}
							${GLFW_LIBRARY})

install(TARGETS atlas
        RUNTIME DESTINATION bin)

# install(TARGETS atlas_i
#         RUNTIME DESTINATION bin)

install(TARGETS atlas
        RUNTIME DESTINATION test)
