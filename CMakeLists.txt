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

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

#################################################
#
#   Environment
#
#################################################
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
option(LIBDS_DEBUG_EVENTS "Print events to console" OFF)
option(LIBDS_DEBUG_PAYLOADS "Print payloads to console" OFF)
option(BUILD_LIBDS_TESTS "Build tests" OFF)

#################################################
#
#   Dependencies
#
#################################################
include(FetchContent)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libteckos EXCLUDE_FROM_ALL)
if (NOT TARGET Pal::Sigslot)
    find_package(PalSigslot REQUIRED)
endif (NOT TARGET Pal::Sigslot)
if (NOT TARGET nlohmann_json::nlohmann_json)
    find_package(nlohmann_json 3.2.0 REQUIRED)
endif (NOT TARGET nlohmann_json::nlohmann_json)


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
        ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Audio/AudioMixer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Audio/AudioMixer.tpp
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
        )
target_link_libraries(${PROJECT_NAME}ApiStatic
        PUBLIC
        teckos::teckosStatic
        Pal::Sigslot
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
        $<INSTALL_INTERFACE:include>
        )
if (CMAKE_COMPILER_IS_GNUCC)
    target_compile_options(${PROJECT_NAME}Auth PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(${PROJECT_NAME}AuthStatic PRIVATE -Wall -Wextra -pedantic -Werror)
endif ()

target_link_libraries(${PROJECT_NAME}Auth
        PUBLIC
        nlohmann_json::nlohmann_json
        teckos::teckos
        )
target_link_libraries(${PROJECT_NAME}AuthStatic
        PUBLIC
        nlohmann_json::nlohmann_json
        teckos::teckosStatic
        )


add_executable(${PROJECT_NAME}-example
        src/cli.cc test/TeckosClientTest.cpp)
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
        TARGETS ${PROJECT_NAME}Api ${PROJECT_NAME}Auth ${PROJECT_NAME}ApiStatic ${PROJECT_NAME}AuthStatic
        NAMESPACE ${PROJECT_NAME}::
        FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
install(
        TARGETS ${PROJECT_NAME}Api ${PROJECT_NAME}Auth ${PROJECT_NAME}ApiStatic ${PROJECT_NAME}AuthStatic
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
if (BUILD_LIBDS_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    # Link runTests with what we want to test and the GTest and pthread library
    add_executable(${PROJECT_NAME}-test
            EXCLUDE_FROM_ALL
            ${CMAKE_CURRENT_SOURCE_DIR}/test/main_test.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/ClientAsyncTest.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/ClientTest.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/TeckosClientTest.cpp
            )
    target_link_libraries(${PROJECT_NAME}-test
            PRIVATE
            GTest::gtest
            GTest::gtest_main
            DigitalStageApiStatic
            DigitalStageAuthStatic
            )
    target_compile_definitions(${PROJECT_NAME}-test PRIVATE DEBUG_TECKOS_SEND)
    target_compile_definitions(${PROJECT_NAME}-test PRIVATE DEBUG_TECKOS_RECV)

    target_compile_definitions(${PROJECT_NAME}-test
            PRIVATE
            AUTH_URL="https://auth.dstage.org"
            API_URL="wss://api.dstage.org"
            )

    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}-test)
endif (BUILD_LIBDS_TESTS)