FILE(GLOB_RECURSE INCLUDE_FILES "*.hpp")

install(FILES ${INCLUDE_FILES}
        DESTINATION include/atlas/core)