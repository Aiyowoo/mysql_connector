
add_custom_target(doc doxygen Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "generate docs"
        SOURCES Doxyfile)