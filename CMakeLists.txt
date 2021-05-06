cmake_minimum_required(VERSION 3.16.0)

### GLOBAL CONFIGURATION
project(DigitalStageApi
        LANGUAGES CXX
        VERSION 0.1
        DESCRIPTION "Library for interacting with the digital stage universe"
        )
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
if (APPLE)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/FixAppleArchitecture.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/BrewResolver.cmake)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)
endif (APPLE)

add_compile_definitions(DEBUG_EVENTS)
add_compile_definitions(DEBUG_PAYLOADS)
add_subdirectory(libteckos EXCLUDE_FROM_ALL)
add_subdirectory(sigslot)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(cpprestsdk REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED)

### SOURCE MANAGEMENT
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

add_library(DigitalStageApi STATIC ${API_SOURCES})
add_library(DigitalStage::Api ALIAS DigitalStageApi)
add_library(DigitalStageAuth STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/AuthService.cc)
add_library(DigitalStage::Auth ALIAS DigitalStageAuth)
target_compile_options(DigitalStageAuth PUBLIC -Wall -Wextra -pedantic -Werror)
target_compile_options(DigitalStageApi PUBLIC -Wall -Wextra -pedantic -Werror)
target_include_directories(DigitalStageApi
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(DigitalStageAuth
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(DigitalStageApi
        PUBLIC
        nlohmann_json::nlohmann_json
        teckos
        Pal::Sigslot
        )
target_link_libraries(DigitalStageAuth
        PUBLIC
        cpprestsdk::cpprest
        OpenSSL::Crypto
        Boost::boost
        )

add_executable(cli
        src/cli.cc)
target_include_directories(cli PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(cli
        PRIVATE
        DigitalStageAuth
        DigitalStageApi)

include(GNUInstallDirs)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/DigitalStage DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS DigitalStageAuth DigitalStageApi
        PUBLIC_HEADER
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/DigitalStage
        )