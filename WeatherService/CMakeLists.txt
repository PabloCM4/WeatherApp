cmake_minimum_required(VERSION 3.28)

project(weather_service)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

add_executable(weather_service
    src/main.cpp
    src/WeatherService.cpp
)

target_include_directories(
    weather_service
    PRIVATE
        include
        ../Crow/include #NEEDS CROW CLONED IN ROOT DIRECTORY
)

target_link_libraries(
    weather_service
    PRIVATE
        CURL::libcurl
)