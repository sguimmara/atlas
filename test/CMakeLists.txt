macro(add_sources DIRNAME GROUPNAME)
  FILE(GLOB ${DIRNAME}_SOURCES "${DIRNAME}/*.cpp")
  FILE(GLOB ${DIRNAME}_HEADERS "${DIRNAME}/*.hpp")
  source_group(${GROUPNAME} FILES ${${DIRNAME}_SOURCES} ${${DIRNAME}_HEADERS})
  set(TEST_SOURCES ${TEST_SOURCES} ${${DIRNAME}_SOURCES} ${${DIRNAME}_HEADERS})
endmacro()

add_sources(core core)
add_sources(core/spatialindex core\\\\spatialindex)
add_sources(core/srs core\\\\srs)

set(TEST_SOURCES ${TEST_SOURCES} tests.cpp)

add_executable(${PROJECT_NAME}test ${TEST_SOURCES})
target_link_libraries(${PROJECT_NAME}test ${GLFW_LIBRARY} ${LIBNAME})