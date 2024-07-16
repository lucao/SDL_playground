cmake_minimum_required(VERSION 3.12.0)
project(game VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)
    
set(SOURCES 
src/engine/CustomGameCharacters.cpp 
src/engine/CustomPhysics.cpp
src/engine/CustomScreen.cpp
src/engine/CustomSDLObject.cpp
src/engine/Stage.cpp
src/engine/World.cpp
src/engine/CustomGameUtils.cpp 
src/engine/CustomGameComponents.cpp)

add_executable(game src/main.cpp ${SOURCES})

target_link_libraries(game PRIVATE SDL2::SDL2 SDL2::SDL2main SDL2_image::SDL2_image)
target_link_libraries(game PRIVATE imgui::imgui)
target_link_libraries(game PRIVATE ${BULLET_LIBRARIES})

target_include_directories(game PUBLIC include)
target_include_directories(game PUBLIC include/macros)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

install(TARGETS game RUNTIME DESTINATION bin)

include(CTest)
enable_testing()

add_executable(smoketest src/test/test.cpp)

add_test(NAME "SmokeTest" COMMAND smoketest)


