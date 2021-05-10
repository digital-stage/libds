cmake_minimum_required(VERSION 3.16.0)

project(DigitalStage
        LANGUAGES CXX
        VERSION 0.1
        DESCRIPTION "Library for interacting with the digital stage universe")


##
## CONFIGURATION
##
include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(DEBUG_EVENTS)
add_compile_definitions(DEBUG_PAYLOADS)

set(TARGET_NAME ${PROJECT_NAME})
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/)
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Install path for include files")
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Install path for library files")

##
## DEPENDENCIES
##
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
if (APPLE)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/FixAppleArchitecture.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/BrewResolver.cmake)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)
endif (APPLE)
add_subdirectory(libteckos EXCLUDE_FROM_ALL)
add_subdirectory(sigslot EXCLUDE_FROM_ALL)
find_package(cpprestsdk REQUIRED)
find_package(OpenSSL REQUIRED)

##
## TARGET
## create target and add include path
##
set(API_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Api/Client.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Api/Events.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Api/Store.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Api/Macros.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage/Types.h
        )
set(API_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Events.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Store.cc
        )
add_library(DigitalStageApi ${API_SOURCES})
add_library(${PROJECT_NAME}::Api ALIAS DigitalStageApi)
target_include_directories(DigitalStageApi PUBLIC
        $<BUILD_INTERFACE:${INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>)
target_compile_options(DigitalStageApi PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(DigitalStageApi
        PUBLIC
        teckos::teckos
        Pal::Sigslot
        )
add_library(DigitalStageAuth ${CMAKE_CURRENT_SOURCE_DIR}/src/AuthService.cc)
add_library(${PROJECT_NAME}::Auth ALIAS DigitalStageAuth)
target_include_directories(DigitalStageAuth PUBLIC
        $<BUILD_INTERFACE:${INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>)
target_compile_options(DigitalStageAuth PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(DigitalStageAuth
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

##
## INSTALL
## install header files, generate and install cmake config files for find_package()
##
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
        TARGETS DigitalStageApi DigitalStageAuth
        NAMESPACE ${PROJECT_NAME}::
        FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
install(
        TARGETS DigitalStageApi DigitalStageAuth
        EXPORT ${TARGETS_EXPORT_NAME}
        INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR})
install(
        EXPORT ${TARGETS_EXPORT_NAME}
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${LIB_INSTALL_DIR}/cmake/${PROJECT_NAME})