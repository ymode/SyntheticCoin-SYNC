cmake_minimum_required(VERSION 3.16)
project(SyntheticCoin VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_WALLET "Build wallet functionality" ON)
option(BUILD_GUI "Build GUI wallet" OFF)
option(ENABLE_HARDENING "Enable hardening features" ON)

# Find packages
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost 1.70 REQUIRED COMPONENTS 
    system 
    filesystem 
    thread 
    program_options 
    chrono
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
)

# Source files
set(CONSENSUS_SOURCES
    src/consensus/params.h
)

set(PODD_SOURCES
    src/podd/device_verifier.h
    src/podd/device_verifier.cpp
)

set(MINING_SOURCES
    src/mining/reward_calculator.h
    src/mining/reward_calculator.cpp
)

set(CORE_SOURCES
    ${CONSENSUS_SOURCES}
    ${PODD_SOURCES}
    ${MINING_SOURCES}
)

# Library targets
add_library(sync_consensus STATIC ${CONSENSUS_SOURCES})
target_compile_features(sync_consensus PUBLIC cxx_std_17)

add_library(sync_podd STATIC ${PODD_SOURCES})
target_link_libraries(sync_podd 
    PUBLIC 
        sync_consensus
        OpenSSL::SSL
        OpenSSL::Crypto
)

add_library(sync_mining STATIC ${MINING_SOURCES})
target_link_libraries(sync_mining 
    PUBLIC 
        sync_consensus
        sync_podd
)

add_library(sync_core STATIC ${CORE_SOURCES})
target_link_libraries(sync_core 
    PUBLIC
        sync_consensus
        sync_podd
        sync_mining
        ${Boost_LIBRARIES}
        Threads::Threads
)

# Executable targets
add_executable(syncd src/syncd.cpp)
target_link_libraries(syncd 
    PRIVATE 
        sync_core
        ${Boost_LIBRARIES}
        OpenSSL::SSL
        OpenSSL::Crypto
        Threads::Threads
)

add_executable(sync-cli src/sync-cli.cpp)
target_link_libraries(sync-cli 
    PRIVATE 
        sync_core
        ${Boost_LIBRARIES}
)

# Installation
install(TARGETS syncd sync-cli
    RUNTIME DESTINATION bin
)

# Testing
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
        -Wno-implicit-fallthrough
    )
    
    if(ENABLE_HARDENING)
        add_compile_options(
            -fstack-protector-strong
            -fPIE
            -D_FORTIFY_SOURCE=2
        )
        add_link_options(
            -pie
            -Wl,-z,relro
            -Wl,-z,now
        )
    endif()
endif()

# Debug/Release flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
    add_compile_options(-g -O0)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)
endif()

# Print configuration
message(STATUS "SyntheticCoin build configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
message(STATUS "  Build Wallet: ${BUILD_WALLET}")
message(STATUS "  Enable Hardening: ${ENABLE_HARDENING}")
