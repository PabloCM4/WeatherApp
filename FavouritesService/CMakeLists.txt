cmake_minimum_required(VERSION 3.28)

project(favourites_service)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Crow CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)

add_executable(favourites_service
    src/main.cpp
    src/FavouritesService.cpp
    src/FavouritesStore.cpp
)

target_include_directories(favourites_service PRIVATE include)

target_link_libraries(
    favourites_service
    PRIVATE
        Crow::Crow
        mongo::mongocxx_static
        mongo::bsoncxx_static
)