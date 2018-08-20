add_executable(test_SceneGraph SceneGraph.cpp)
install(TARGETS test_SceneGraph
        RUNTIME DESTINATION test)

add_test(NAME test_SceneGraph
         COMMAND test_SceneGraph)