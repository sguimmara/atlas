add_executable(test_Cartographic Cartographic.cpp)
install(TARGETS test_Cartographic
        RUNTIME DESTINATION test)

add_test(NAME test_Cartographic
         COMMAND test_Cartographic)