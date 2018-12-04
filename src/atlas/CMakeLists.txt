macro(add_sources DIRNAME GROUPNAME)
  FILE(GLOB ${DIRNAME}_SOURCES "${DIRNAME}/*.cpp")
  FILE(GLOB ${DIRNAME}_HEADERS "${DIRNAME}/*.hpp")
  source_group(${GROUPNAME} FILES ${${DIRNAME}_SOURCES} ${${DIRNAME}_HEADERS})
  set(SOURCES ${SOURCES} ${${DIRNAME}_SOURCES} ${${DIRNAME}_HEADERS})
  install(FILES ${${DIRNAME}_HEADERS}
          DESTINATION include/atlas/${DIRNAME})
endmacro()

add_sources(core core)
add_sources(io io)
add_sources(renderer renderer)
add_sources(scene scene)
add_sources(viewer viewer)

add_library(${LIBNAME} SHARED ${SOURCES})
target_link_libraries(${LIBNAME} ${Vulkan_LIBRARY} ${GLFW_LIBRARY} SPIRVCROSS json)
