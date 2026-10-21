cmake_minimum_required(VERSION 3.28)

project(favourites_service)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG REQUIRED)

add_executable(favourites_service
    src/main.cpp
    src/FavouritesService.cpp
)

target_include_directories(favourites_service PRIVATE include)

target_link_libraries(
    favourites_service
    PRIVATE
        Crow::Crow
)