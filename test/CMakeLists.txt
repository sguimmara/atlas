macro(add_sources DIRNAME GROUPNAME)
  FILE(GLOB ${DIRNAME}_SOURCES "${DIRNAME}/*.cpp")
  FILE(GLOB ${DIRNAME}_HEADERS "${DIRNAME}/*.hpp")
  source_group(${GROUPNAME} FILES ${${DIRNAME}_SOURCES} ${${DIRNAME}_HEADERS})
  set(TEST_SOURCES ${TEST_SOURCES} ${${DIRNAME}_SOURCES} ${${DIRNAME}_HEADERS})
endmacro()

add_sources(core core)
# add_sources(core/srs core\\\\srs)
add_sources(core/spatialindex core\\\\spatialindex)
# add_sources(io io)
# add_sources(renderer renderer)
# add_sources(scene scene)
# add_sources(viewer viewer)

set(TEST_SOURCES ${TEST_SOURCES} tests.cpp)

add_executable(atlas_test ${TEST_SOURCES})
target_link_libraries(atlas_test ${GLFW_LIBRARY} ${LIBNAME})