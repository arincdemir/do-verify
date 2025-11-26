
add_executable(unit_tests 
    test_logic.cpp 
    # test_main.cpp # Include only if you have a custom main
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
catch_discover_tests(unit_tests)