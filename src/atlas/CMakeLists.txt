set(SOURCES "")

macro(add_sources DIRNAME)
  FILE(GLOB ${DIRNAME}_SOURCES "${DIRNAME}/*.cpp")
  FILE(GLOB ${DIRNAME}_HEADERS "${DIRNAME}/*.hpp")
  source_group(${DIRNAME} FILES ${${DIRNAME}_SOURCES} ${${DIRNAME}_HEADERS})
  set(SOURCES ${SOURCES} ${${DIRNAME}_SOURCES} ${${DIRNAME}_HEADERS})
  install(FILES ${${DIRNAME}_HEADERS}
          DESTINATION include/atlas/${DIRNAME})
endmacro()

add_sources(core)
add_sources(util)
add_sources(graphics)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(atlas OBJECT ${SOURCES})

set(ATLAS_VERSION_MAJOR 0)
set(ATLAS_VERSION_MINOR 1)
set(ATLAS_VERSION_PATCH 0)
set(ATLAS_VERSION_STRING ${ATLAS_VERSION_MAJOR}.${ATLAS_VERSION_MINOR}.${ATLAS_VERSION_PATCH})

set(LIBNAME atlas)

macro(create_library NAME TYPE SOURCES)
  add_library(${NAME} ${TYPE} ${SOURCES})
  set_target_properties(${NAME} PROPERTIES OUTPUT_NAME atlas)
  set_target_properties(${NAME} PROPERTIES VERSION ${ATLAS_VERSION_STRING} SOVERSION ${ATLAS_VERSION_MAJOR})
  target_link_libraries(${NAME} ${Vulkan_LIBRARY} ${GLFW_LIBRARY})
endmacro()

create_library(atlas_dll  SHARED $<TARGET_OBJECTS:atlas>)
install(TARGETS atlas_dll RUNTIME DESTINATION bin)

create_library(atlas_static STATIC $<TARGET_OBJECTS:atlas>)
install(TARGETS atlas_static ARCHIVE DESTINATION lib)
