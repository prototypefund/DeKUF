cmake_minimum_required(VERSION 3.16)

project(dekuf-client VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Test)

add_executable(dekuf-client
    src/main.cpp
    src/client.cpp
)
target_link_libraries(dekuf-client PRIVATE Qt6::Core Qt6::Network)

enable_testing(true)
add_executable(dekuf-client-tests
    tests/main.cpp
    tests/survey_test.cpp
)
target_link_libraries(dekuf-client-tests PRIVATE Qt::Test)
add_test(NAME dekuf-client-tests COMMAND dekuf-client-tests)
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
