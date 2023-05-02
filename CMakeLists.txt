cmake_minimum_required(VERSION 3.0.0)
project(game VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
    
set(SOURCES 
src/custom_classes/src/CustomCollision.cpp 
src/custom_classes/src/CustomGameCharacters.cpp 
src/custom_classes/src/CustomScreen.cpp
src/custom_classes/src/CustomSDLObject.cpp
src/custom_classes/src/Stage.cpp
src/custom_classes/src/World.cpp
)

add_executable(game src/main.cpp ${SOURCES})

target_link_libraries(game
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    )
target_link_libraries(game PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
target_link_libraries(game PRIVATE imgui::imgui)
target_include_directories(game PUBLIC include)
target_include_directories(game PUBLIC include/macros)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
