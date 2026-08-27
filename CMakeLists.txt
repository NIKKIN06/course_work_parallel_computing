cmake_minimum_required(VERSION 3.16)

project(course_work_parallel_computing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-30-2
)

FetchContent_MakeAvailable(asio)

set(ASIO_INCLUDE_DIR "${asio_SOURCE_DIR}/asio/include" CACHE PATH "Path to ASIO" FORCE)

set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "Disable examples" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "Disable tests" FORCE)

FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG master
)
FetchContent_MakeAvailable(crow)

add_executable(search_server src/main.cpp)

target_link_libraries(search_server PRIVATE Crow::Crow)