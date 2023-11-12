# Allow short runs during automated testing to see if something new breaks
set(FUZZ_RUNTIME
    10
    CACHE STRING "Number of seconds to run fuzz tests during ctest run") # Default of 10 seconds

find_package(Sanitizers)


function(add_fuzzing_target main_name)
    set(target_name fuzzing_${main_name})
    add_executable(${target_name} ${main_name}.cpp)
    target_compile_features(${target_name} PUBLIC cxx_std_20)
    target_link_libraries(${target_name} PUBLIC IncludeFilter::IncludeFilterLib )

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_link_libraries(
        ${target_name}
      PRIVATE
              -coverage
              -fsanitize=fuzzer)
	target_compile_options(${target_name} PRIVATE -fsanitize=fuzzer)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	target_compile_options(${target_name} PRIVATE /fsanitize=fuzzer)
endif()
    add_sanitizers(${target_name})
    
    add_test(NAME ${target_name}_run COMMAND ${target_name} -max_total_time=${FUZZ_RUNTIME} -timeout=${FUZZ_RUNTIME}) 
endfunction()

add_fuzzing_target(msvc)
add_fuzzing_target(clang)


