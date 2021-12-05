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
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
option(LIBDS_DEBUG_EVENTS "Print events to console" OFF)
option(LIBDS_DEBUG_PAYLOADS "Print payloads to console" OFF)
option(BUILD_TESTS "Build tests" OFF)

#################################################
#
#   Dependencies
#
#################################################
include(FetchContent)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libteckos EXCLUDE_FROM_ALL)
if (NOT TARGET sigslot)
    FetchContent_Declare(
            sigslot
            GIT_REPOSITORY https://github.com/palacaze/sigslot
            GIT_TAG e41c6e62edfa67b6402b17a627dca698a14f47a8 # v1.2.1
    )
    FetchContent_MakeAvailable(sigslot)
endif (NOT TARGET sigslot)
if (NOT TARGET plog)
    FetchContent_Declare(
            plog
            GIT_REPOSITORY https://github.com/SergiusTheBest/plog.git
    )
    FetchContent_MakeAvailable(plog)
endif (NOT TARGET plog)


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
add_library(DigitalStageApi SHARED ${API_SOURCES} ${API_HEADERS})
add_library(${PROJECT_NAME}::Api ALIAS ${PROJECT_NAME}Api)
add_library(DigitalStageApiStatic STATIC ${API_SOURCES} ${API_HEADERS})
add_library(${PROJECT_NAME}::ApiStatic ALIAS ${PROJECT_NAME}ApiStatic)
target_include_directories(${PROJECT_NAME}Api PUBLIC
        $<BUILD_INTERFACE:${INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>)
target_include_directories(${PROJECT_NAME}ApiStatic PUBLIC
        $<BUILD_INTERFACE:${INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>)
if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(${PROJECT_NAME}Api PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(${PROJECT_NAME}Static PRIVATE -Wall -Wextra -pedantic -Werror)
endif ()
if (LIBDS_DEBUG_EVENTS)
    target_compile_definitions(${PROJECT_NAME}Api PRIVATE DEBUG_EVENTS)
    if (LIBDS_DEBUG_PAYLOADS)
        target_compile_definitions(${PROJECT_NAME}Api PRIVATE DEBUG_PAYLOADS)
    endif (LIBDS_DEBUG_PAYLOADS)
endif (LIBDS_DEBUG_EVENTS)
target_link_libraries(${PROJECT_NAME}Api
        PUBLIC
        teckos::teckos
        Pal::Sigslot
        plog
        )
target_link_libraries(${PROJECT_NAME}ApiStatic
        PUBLIC
        teckos::teckosStatic
        Pal::Sigslot
        plog
        )


#################################################
#
#   Target: Auth
#
#################################################
add_library(DigitalStageAuth SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/AuthService.cc ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Auth/AuthService.h ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Auth/AuthError.h)
add_library(${PROJECT_NAME}::Auth ALIAS ${PROJECT_NAME}Auth)
add_library(DigitalStageAuthStatic STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/AuthService.cc ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Auth/AuthService.h ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Auth/AuthError.h)
add_library(${PROJECT_NAME}::AuthStatic ALIAS ${PROJECT_NAME}AuthStatic)
target_include_directories(${PROJECT_NAME}Auth PUBLIC
        $<BUILD_INTERFACE:${INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>)
target_include_directories(${PROJECT_NAME}AuthStatic PUBLIC
        $<BUILD_INTERFACE:${INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>)
if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(${PROJECT_NAME}Auth PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(${PROJECT_NAME}AuthStatic PRIVATE -Wall -Wextra -pedantic -Werror)
endif ()
target_link_libraries(${PROJECT_NAME}Auth
        PUBLIC
        nlohmann_json::nlohmann_json
        teckos::teckos
        plog
        )
target_link_libraries(${PROJECT_NAME}AuthStatic
        PUBLIC
        nlohmann_json::nlohmann_json
        teckos::teckosStatic
        plog
        )


add_executable(${PROJECT_NAME}-example
        src/cli.cc)
target_compile_definitions(${PROJECT_NAME}-example
        PUBLIC
        )
target_compile_definitions(${PROJECT_NAME}-example
        PRIVATE
        DEBUG_EVENTS
        DEBUG_PAYLOADS)
target_include_directories(${PROJECT_NAME}-example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME}-example
        PRIVATE
        ${PROJECT_NAME}AuthStatic
        ${PROJECT_NAME}ApiStatic)


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
        TARGETS ${PROJECT_NAME}Api ${PROJECT_NAME}Auth sigslot ${PROJECT_NAME}ApiStatic ${PROJECT_NAME}AuthStatic plog
        NAMESPACE ${PROJECT_NAME}::
        FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
install(
        TARGETS ${PROJECT_NAME}Api ${PROJECT_NAME}Auth sigslot ${PROJECT_NAME}ApiStatic ${PROJECT_NAME}AuthStatic plog
        EXPORT ${TARGETS_EXPORT_NAME}
        INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR})
install(
        EXPORT ${TARGETS_EXPORT_NAME}
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME})


#################################################
#
#   Tests
#
#################################################
if (target GTest)
    find_package(GTest REQUIRED)
    enable_testing()

    # Link runTests with what we want to test and the GTest and pthread library
    add_executable(${PROJECT_NAME}-test
            EXCLUDE_FROM_ALL
            ${CMAKE_CURRENT_SOURCE_DIR}/test/main_test.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/AuthServiceTest.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/ClientAsyncTest.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/ClientTest.cpp
            )
    target_link_libraries(${PROJECT_NAME}-test
            PRIVATE
            GTest::gtest
            GTest::gtest_main
            DigitalStageApiStatic
            DigitalStageAuthStatic
            )

    target_compile_definitions(${PROJECT_NAME}-test
            PRIVATE
            AUTH_URL="https://auth.dstage.org"
            API_URL="wss://api.dstage.org"
            )

    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}-test)
endif (target GTest)