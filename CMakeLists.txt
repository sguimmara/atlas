cmake_minimum_required(VERSION 3.7.0 FATAL_ERROR)
if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif()

set(PROJECT_NAME atlas)

set(LIBNAME lib${PROJECT_NAME})

project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES CXX)

add_definitions( -DAPP_VERSION="${PROJECT_VERSION}" )
add_definitions( -DAPP_NAME="${PROJECT_NAME}" )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

if (CMAKE_CXX_COMPILER MATCHES ".*clang")
    set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()

if(MSVC)
    SET(PLATFORM_CXX_FLAGS "/W3 /WX /Gs /DNOMINMAX /EHsc /Dlest_FEATURE_COLOURISE#1 /Dlest_FEATURE_AUTO_REGISTER#1")
    SET(PLATFORM_CXX_FLAGS_DEBUG "/MDd /Zi /Od")
    SET(PLATFORM_CXX_FLAGS_RELEASE "/MD /O2")
endif(MSVC)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
    SET(PLATFORM_CXX_FLAGS "${COMPILER_SPECIFIC_CXX_FLAGS} -pedantic -Werror -Wall -Dlest_FEATURE_COLOURISE -Dlest_FEATURE_AUTO_REGISTER")
    SET(PLATFORM_CXX_FLAGS_DEBUG "-g -O0")
    SET(PLATFORM_CXX_FLAGS_RELEASE "-O3")
endif()

set (CMAKE_CXX_FLAGS ${PLATFORM_CXX_FLAGS})
set (CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} ${PLATFORM_CXX_FLAGS_DEBUG} -DDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${PLATFORM_CXX_FLAGS_RELEASE}")

# use the gold linker if available
if (UNIX AND NOT APPLE)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -fuse-ld=gold -Wl,--version ERROR_QUIET OUTPUT_VARIABLE ld_version)
    if ("${ld_version}" MATCHES "GNU gold")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold -Wl,--disable-new-dtags")
    endif()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(GLFW REQUIRED)
find_package(Vulkan REQUIRED)

if (Vulkan_FOUND)
    add_definitions(-DENABLE_VULKAN)
    include_directories(${GLFW_INCLUDE_DIR})
    include_directories(${Vulkan_INCLUDE_DIR})
else()
    message("Vulkan NOT SUPPORTED on this build. Using pseudorendering instead.")
endif()

include_directories(SYSTEM lib)

# include_directories(test)
include_directories(src)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib)
add_subdirectory(apps)
add_subdirectory(shaders)
