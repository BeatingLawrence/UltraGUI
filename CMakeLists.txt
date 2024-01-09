cmake_minimum_required(VERSION 3.14)

#[[ UltraGUI versioning system:

    MAJOR.MINOR.PATCH.TYPE

    MAJOR:  Substantial breaking changes
    MINOR:  Substantial non-breaking changes
    PATCH:  Bug fixes, optimization changes
    TYPE:   Version type. This number is mapped to the
            following values when the gui::UltraGUI::version()
            function is called:
            0:  Alpha
            1:  Beta
            99: Release
            Using a different number will cause a build failure

            Code cleaning must not change the version in any of its numbers

                                        Version
                                          ||
#]]#                                      V

project(UltraGUI LANGUAGES CXX VERSION 1.0.1.1)
message(STATUS ${CMAKE_PROJECT_VERSION})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_DEBUG_POSTFIX -d)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "../Dist" CACHE PATH "..." FORCE)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# =======================UltraGUI library=========================

file(GLOB_RECURSE src_files CONFIGURE_DEPENDS
    "src/*.cpp"
)

file(GLOB_RECURSE header_files CONFIGURE_DEPENDS
    "src/*.h"
)

qt_add_library(UltraGUI SHARED
    ${src_files}
    ${header_files}
)

target_link_libraries(UltraGUI PRIVATE Qt6::Core Qt6::Widgets)

set_target_properties(UltraGUI PROPERTIES PREFIX "lib" IMPORT_PREFIX "lib")

target_compile_definitions(UltraGUI PRIVATE ULTRAGUI_LIBRARY PRIVATE _CRT_SECURE_NO_WARNINGS PRIVATE LIBRARY_VERSION="${CMAKE_PROJECT_VERSION}")
target_compile_definitions(UltraGUI PRIVATE ULTRAGUI_LIBRARY PRIVATE _CRT_SECURE_NO_WARNINGS PRIVATE LIBRARY_VERSION_TWEAK=${CMAKE_PROJECT_VERSION_TWEAK})

install(TARGETS UltraGUI DESTINATION lib)
install(DIRECTORY src/ DESTINATION include/ultragui FILES_MATCHING PATTERN "*.h")

# =======================test executable==========================

file(GLOB_RECURSE test_files CONFIGURE_DEPENDS
    "testApplication/*"
)

add_executable(TestAPP EXCLUDE_FROM_ALL
  ${test_files}
)

target_include_directories(TestAPP PUBLIC
    "../Dist/include"
)

target_link_directories(TestAPP PUBLIC "../Dist/lib")
target_link_libraries(TestAPP PRIVATE UltraGUI-d Qt6::Core Qt6::Widgets)

# ================================================================

if(MSVC)

    message(STATUS "On Windows system")

    set_target_properties(UltraGUI test PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL"
    )

    target_compile_definitions(UltraGUI PRIVATE WINDOWS_SYSTEM)

elseif(UNIX)

    if(APPLE)
        target_compile_definitions(UltraGUI PRIVATE APPLE_SYSTEM)
    else()
        target_compile_definitions(UltraGUI PRIVATE LINUX_SYSTEM)
    endif()

    message(STATUS "On Linux/MACOSX system")

endif()
