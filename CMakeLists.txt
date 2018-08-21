cmake_minimum_required(VERSION 3.7.0 FATAL_ERROR)
if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif()

project(atlas VERSION 0.1.0 LANGUAGES CXX)

add_definitions( -DAPP_VERSION="${PROJECT_VERSION}" )
add_definitions( -DAPP_NAME="${PROJECT_NAME}" )

set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER MATCHES ".*clang")
    set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()

if(MSVC)
    SET(PLATFORM_CXX_FLAGS "/W3 /WX /Gs /DNOMINMAX /EHsc /Dlest_FEATURE_COLOURISE#1")
    SET(PLATFORM_CXX_FLAGS_DEBUG "/MDd /Zi /Od")
    SET(PLATFORM_CXX_FLAGS_RELEASE "/MD /O2")
endif(MSVC)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
    SET(PLATFORM_CXX_FLAGS "${COMPILER_SPECIFIC_CXX_FLAGS} -pedantic -Werror -Wall -D lest_FEATURE_COLOURISE")
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
include_directories(${GLFW_INCLUDE_DIR})

find_package(GDAL REQUIRED)
include_directories(${GDAL_INCLUDE_DIR})

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIR})

find_package(Boost 1.68.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(Doxygen)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/third_party)

add_subdirectory(include)
add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(apps)

# if (CMAKE_BUILD_TYPE MATCHES Debug)
    enable_testing()
    add_subdirectory(test)
# endif()
