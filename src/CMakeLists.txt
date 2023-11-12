find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
set(CMAKE_AUTORCC ON)
qt_standard_project_setup()

add_library(IncludeFilterLib)
add_library(IncludeFilter::IncludeFilterLib ALIAS IncludeFilterLib)
target_sources(IncludeFilterLib PRIVATE
"ParseIncludes.cpp"
"ui/mainwindow.cpp"
"ui/mainwindow.ui")
target_sources(IncludeFilterLib PUBLIC FILE_SET includes TYPE HEADERS FILES 
"ParseIncludes.h"
"ParseIncludes_Testing.h"
"ModelIndexIterator.h")
target_link_libraries(IncludeFilterLib PUBLIC Qt6::Core Qt6::Widgets )
target_compile_features(IncludeFilterLib PUBLIC cxx_std_20)
target_compile_definitions(IncludeFilterLib PUBLIC "VERSION=\"${CMAKE_PROJECT_VERSION}\"")

find_package(Sanitizers)
add_sanitizers(IncludeFilterLib)


qt_add_executable(IncludeFilter "main.cpp" "rc.qrc")
target_link_libraries(IncludeFilter PRIVATE IncludeFilterLib)
set_target_properties(IncludeFilter PROPERTIES WIN32_EXECUTABLE TRUE)

install(TARGETS IncludeFilter
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET IncludeFilter
    OUTPUT_SCRIPT deploy_script
    NO_TRANSLATIONS
    NO_UNSUPPORTED_PLATFORM_ERROR
    PRE_EXCLUDE_REGEXES d3compiler* opengl32* *Network* *Svg*
    POST_EXCLUDE_FILES "d3compiler_47.dll;opengl32sw.dll;Qt6Network.dll;Qt6Svg.dll"
)
install(SCRIPT ${deploy_script})

include(CPack)
ADD_CUSTOM_TARGET( bundle
         COMMAND "${CMAKE_CPACK_COMMAND}"
                 "-C" "$<CONFIGURATION>" "-G" "7Z"
            "--config" "${CMAKE_BINARY_DIR}/CPackConfig.cmake"
            DEPENDS IncludeFilter)