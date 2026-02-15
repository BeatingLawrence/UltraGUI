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
cmake_minimum_required(VERSION 3.16)

project(UltraGUI LANGUAGES CXX VERSION 1.0.1.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_DEBUG_POSTFIX -d)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "../dist" CACHE PATH "Install path" FORCE)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Qt 6.6 + recent macOS SDKs may expose legacy AGL through WrapOpenGL.
# New SDKs do not provide AGL anymore, so strip only that framework.
if(APPLE AND TARGET WrapOpenGL::WrapOpenGL)
    get_target_property(_wrap_opengl_libs WrapOpenGL::WrapOpenGL INTERFACE_LINK_LIBRARIES)
    if(_wrap_opengl_libs)
        list(FILTER _wrap_opengl_libs EXCLUDE REGEX "AGL\\.framework")
        set_target_properties(WrapOpenGL::WrapOpenGL PROPERTIES
            INTERFACE_LINK_LIBRARIES "${_wrap_opengl_libs}")
    endif()
endif()

file(GLOB_RECURSE ULTRAGUI_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE ULTRAGUI_HEADERS CONFIGURE_DEPENDS "src/*.h")

qt_add_library(UltraGUI SHARED)

target_sources(UltraGUI PRIVATE
    ${ULTRAGUI_SOURCES}
    ${ULTRAGUI_HEADERS}
)

target_include_directories(UltraGUI
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
        "$<INSTALL_INTERFACE:include/ultragui>"
)

target_link_libraries(UltraGUI
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)

target_compile_definitions(UltraGUI
    PRIVATE
        ULTRAGUI_LIBRARY
        LIBRARY_VERSION="${CMAKE_PROJECT_VERSION}"
        LIBRARY_VERSION_TWEAK=${CMAKE_PROJECT_VERSION_TWEAK}
)

if(MSVC)
    target_compile_definitions(UltraGUI PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

set_target_properties(UltraGUI PROPERTIES PREFIX "lib" IMPORT_PREFIX "lib")

install(TARGETS UltraGUI DESTINATION lib)
install(DIRECTORY src/ DESTINATION include/ultragui FILES_MATCHING PATTERN "*.h")

file(GLOB_RECURSE TESTAPP_SOURCES CONFIGURE_DEPENDS "testApp/*")

add_executable(TestAPP EXCLUDE_FROM_ALL ${TESTAPP_SOURCES})
target_link_libraries(TestAPP PRIVATE UltraGUI)
