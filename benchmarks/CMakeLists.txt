add_executable(bench_runner 
    benchmark_engine.cpp
)

# Link the SAME library and SAME Catch2
target_link_libraries(bench_runner PRIVATE do-verify Catch2::Catch2WithMain)

# Copy Data: Root/data -> build/benchmarks/data
# We copy it here too, so the benchmark executable can find it locally
add_custom_command(TARGET bench_runner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:bench_runner>/data
)