cmake_minimum_required(VERSION 3.16.0)

project(DigitalStage
        LANGUAGES CXX
        VERSION 0.2
        DESCRIPTION "Library for interacting with the digital stage universe")


#################################################
#
#   Common configuration
#
#################################################
include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(TARGET_NAME ${PROJECT_NAME})
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/)
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Install path for include files")
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Install path for library files")

if (APPLE)
    if (EXISTS "/usr/local/opt/openssl/")
        set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl/")
    else ()
        include("${PROJECT_SOURCE_DIR}/cmake/BrewResolver.cmake")
    endif ()
endif (APPLE)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

## Windows setup for Boost
## If you wonder WTF that is about, check https://stackoverflow.com/questions/9742003/platform-detection-in-cmake
if (WIN32)
    macro(get_WIN32_WINNT version)
        if (CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
            string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
            # Check for Windows 10, b/c we'll need to convert to hex 'A'.
            if ("${verMajor}" MATCHES "10")
                set(verMajor "A")
                string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
            endif ()
            # Remove all remaining '.' characters.
            string(REPLACE "." "" ver ${ver})
            # Prepend each digit with a zero.
            string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
            set(${version} "0x${ver}")
        endif ()
    endmacro()

    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})
endif ()


#################################################
#
#   Environment
#
#################################################
option(DEBUG_EVENTS "Print events to console" OFF)
option(DEBUG_PAYLOADS "Print payloads to console" OFF)


#################################################
#
#   Dependencies
#
#################################################
add_subdirectory(libteckos EXCLUDE_FROM_ALL)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/FetchAndPatchSigslot.cmake)
find_package(cpprestsdk REQUIRED)
find_package(OpenSSL REQUIRED)


#################################################
#
#   Target: API
#
#################################################
set(API_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Api/Client.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Api/Events.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Api/Store.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Types.h
        )
set(API_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Events.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Store.cc
        )
add_library(DigitalStageApi ${API_SOURCES} ${API_HEADERS})
add_library(${PROJECT_NAME}::Api ALIAS DigitalStageApi)
add_library(DigitalStageApiStatic STATIC ${API_SOURCES} ${API_HEADERS})
add_library(${PROJECT_NAME}::ApiStatic ALIAS DigitalStageApiStatic)
target_include_directories(DigitalStageApi PUBLIC
        $<BUILD_INTERFACE:${INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>)
target_include_directories(DigitalStageApiStatic PUBLIC
        $<BUILD_INTERFACE:${INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>)
if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(DigitalStageApi PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(DigitalStageApiStatic PRIVATE -Wall -Wextra -pedantic -Werror)
endif ()
target_link_libraries(DigitalStageApi
        PUBLIC
        teckos::teckos
        Pal::Sigslot
        )
target_link_libraries(DigitalStageApiStatic
        PUBLIC
        teckos::teckosStatic
        Pal::Sigslot
        )


#################################################
#
#   Target: Auth
#
#################################################
add_library(DigitalStageAuth src/AuthService.cc include/DigitalStage/Auth/AuthService.h)
add_library(${PROJECT_NAME}::Auth ALIAS DigitalStageAuth)
add_library(DigitalStageAuthStatic STATIC src/AuthService.cc include/DigitalStage/Auth/AuthService.h)
add_library(${PROJECT_NAME}::AuthStatic ALIAS DigitalStageAuthStatic)
target_include_directories(DigitalStageAuth PUBLIC
        $<BUILD_INTERFACE:${INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>)
target_include_directories(DigitalStageAuthStatic PUBLIC
        $<BUILD_INTERFACE:${INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>)
if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(DigitalStageAuth PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(DigitalStageAuthStatic PRIVATE -Wall -Wextra -pedantic -Werror)
endif ()
target_link_libraries(DigitalStageAuth
        PUBLIC
        cpprestsdk::cpprest
        OpenSSL::Crypto
        )
target_link_libraries(DigitalStageAuthStatic
        PUBLIC
        cpprestsdk::cpprest
        OpenSSL::Crypto
        )

add_executable(libds-example
        src/cli.cc)
set_target_properties(libds-example PROPERTIES OUTPUT_NAME cli)
target_include_directories(libds-example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(libds-example
        PRIVATE
        DigitalStageAuth
        DigitalStageApi)


#################################################
#
#   Installation
#
#################################################
include(CMakePackageConfigHelpers)
configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${LIB_INSTALL_DIR}/${PROJECT_NAME}/cmake
        PATH_VARS INCLUDE_INSTALL_DIR)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME})
install(
        DIRECTORY ${INCLUDE_DIR}
        DESTINATION ${INCLUDE_INSTALL_DIR}
)
export(
        TARGETS DigitalStageApi DigitalStageAuth sigslot DigitalStageApiStatic DigitalStageAuthStatic
        NAMESPACE ${PROJECT_NAME}::
        FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
install(
        TARGETS DigitalStageApi DigitalStageAuth sigslot DigitalStageApiStatic DigitalStageAuthStatic
        EXPORT ${TARGETS_EXPORT_NAME}
        INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR})
install(
        EXPORT ${TARGETS_EXPORT_NAME}
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME})
