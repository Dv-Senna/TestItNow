cmake_minimum_required(VERSION 3.30)

project(TestItNow
	VERSION 0.1.0
	LANGUAGES CXX
)

find_package(CLIser 0.2 REQUIRED COMPONENTS DEVEL)

add_library(TestItNow INTERFACE)
add_library(TestItNow::TestItNow ALIAS TestItNow)

target_include_directories(TestItNow INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(TestItNow INTERFACE cxx_std_26)
target_link_Libraries(TestItNow INTERFACE CLIser::CLIser)
target_compile_definitions(TestItNow INTERFACE TestItNow_VERSION="${TestItNow_VERSION}")


file(GLOB_RECURSE EXAMPLE_SOURCES examples/*.cpp)
add_executable(examples ${EXAMPLE_SOURCES})
target_link_libraries(examples TestItNow::TestItNow)
