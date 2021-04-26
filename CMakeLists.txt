cmake_minimum_required(VERSION 3.16.0)

### GLOBAL CONFIGURATION
project(ds
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

#add_compile_definitions(DEBUG_EVENTS)
#add_compile_definitions(DEBUG_PAYLOADS)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(cpprestsdk REQUIRED)
find_package(OpenSSL REQUIRED)
add_library(eventpp INTERFACE)
target_compile_options(eventpp INTERFACE -Wall -Wextra -pedantic -Werror)
target_include_directories(eventpp INTERFACE ${CMAKE_CURRENT_LIST_DIR}/third_party/eventpp/include)


### SOURCE MANAGEMENT
set(HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ds/AuthService.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ds/Client.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ds/Events.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ds/Types.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ds/Store.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ds/Macros.h
        )
set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/AuthService.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Client.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Events.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Store.cc
        )

add_library(ds STATIC ${SOURCES})
target_compile_options(ds PRIVATE -Wall -Wextra -pedantic -Werror)
set_target_properties(ds PROPERTIES PUBLIC_HEADER "${HEADER}")
target_include_directories(ds 
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ds)
target_link_libraries(ds
        PUBLIC
        nlohmann_json::nlohmann_json
        OpenSSL::Crypto
        cpprestsdk::cpprest
        eventpp)

add_executable(cli 
        src/cli.cc)
target_include_directories(cli PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(cli ds)

install(FILES ${HEADER} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ds)