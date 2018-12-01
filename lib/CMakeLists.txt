add_subdirectory(json)

include_directories(SYSTEM spirv-cross)
FILE(GLOB SPIRVCROSS_SOURCES "spirv-cross/*.cpp")
add_library(SPIRVCROSS STATIC ${SPIRVCROSS_SOURCES})