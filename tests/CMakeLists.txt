

find_package(Qt6 REQUIRED COMPONENTS Test)
qt_standard_project_setup()


# Add source to this project's executable.
qt_add_executable(IncludeFilter_Tests)
target_link_libraries(IncludeFilter_Tests PUBLIC Qt6::Test IncludeFilter::IncludeFilterLib)
target_sources(IncludeFilter_Tests PRIVATE
"main.cpp"
"ParseIncludes_Tests.cpp"
"ModelIndexIterator_Tests.h")
target_compile_definitions(IncludeFilter_Tests PRIVATE "TEST_DATA=\"${CMAKE_CURRENT_SOURCE_DIR}/test_data/\"")
target_compile_features(IncludeFilter_Tests PUBLIC cxx_std_20)

find_package(Sanitizers)
add_sanitizers(IncludeFilter_Tests)

add_test(NAME IncludeFilter_Tests COMMAND IncludeFilter_Tests)

