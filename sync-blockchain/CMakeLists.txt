cmake_minimum_required(VERSION 3.16)
project(SyntheticCoin VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(Boost REQUIRED COMPONENTS system filesystem thread program_options)
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(src)

# Build the SYNC daemon
add_executable(syncd 
    src/sync-main.cpp
    src/podd/device_verifier.cpp
    src/mining/reward_calculator.cpp
)

target_link_libraries(syncd 
    ${Boost_LIBRARIES} 
    OpenSSL::SSL 
    OpenSSL::Crypto
    pthread
)

# Build the SYNC CLI
add_executable(sync-cli src/sync-cli.cpp)
target_link_libraries(sync-cli ${Boost_LIBRARIES})
