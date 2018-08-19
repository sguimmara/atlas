add_subdirectory(util)
add_subdirectory(core)

add_library(atlas SHARED util
                         core
           )

target_link_libraries(atlas
                      util)

install(TARGETS atlas
        LIBRARY DESTINATION lib)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    enable_testing()
    add_subdirectory(test)
endif()
