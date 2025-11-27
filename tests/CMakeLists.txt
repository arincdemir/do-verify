if(ENABLE_COVERAGE)
    find_program(GCOVR_PATH gcovr)

    if(GCOVR_PATH)
        add_custom_target(coverage
            # 1. Run the tests
            COMMAND ./unit_tests
            
            # 2. Make directory
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
            
            # 3. Generate Report
            COMMAND ${GCOVR_PATH} 
                -r ${CMAKE_SOURCE_DIR} 
                --filter "${CMAKE_SOURCE_DIR}/include/"
                --exclude "${CMAKE_BINARY_DIR}/_deps/"
                --gcov-ignore-parse-errors
                --gcov-ignore-errors=all
                --exclude-throw-branches
                --html --html-details 
                -o ${CMAKE_BINARY_DIR}/coverage/index.html
            
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating coverage report..."
        )
    endif()
endif()



add_executable(unit_tests 
    test_dense.cpp
    test_interval_set.cpp
)

target_link_libraries(unit_tests PRIVATE do-verify Catch2::Catch2WithMain)

# Copy Data: Root/data -> build/tests/data
add_custom_command(TARGET unit_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:unit_tests>/data
)

include(CTest)
include(Catch)
