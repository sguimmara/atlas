if (DOXYGEN_FOUND)
    set(DOC_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doc ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    FILE(GLOB_RECURSE HTML_DOC "${DOC_DIR}/doc/html/*")

    install(FILES ${HTML_DOC}
            DESTINATION doc)
else(DOXYGEN_FOUND)
    message("Doxygen needs to be installed to generate the doxygen documentation.")
endif(DOXYGEN_FOUND)