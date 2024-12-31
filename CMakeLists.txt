cmake_minimum_required(VERSION 3.12.0)
project(game VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)
    
set(SOURCES 
src/engine/CustomPlayerBuilder.cpp 
src/engine/CustomGameCharacters.cpp 
src/engine/CustomPhysics.cpp
src/engine/CustomScreen.cpp
src/engine/CustomSDLObjects.cpp
src/engine/Stage.cpp
src/engine/World.cpp
src/engine/CustomGameObjects.cpp 
src/engine/CustomGameUtils.cpp 
src/engine/CustomGameComponents.cpp 
src/engine/CustomTexture.cpp)

add_executable(game src/main.cpp ${SOURCES})

link_directories(libs)

target_link_libraries(game PRIVATE d3d9)
target_link_libraries(game
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    )
target_link_libraries(game PRIVATE $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
target_link_libraries(game PRIVATE imgui::imgui)
target_link_libraries(game PRIVATE ${BULLET_LIBRARIES})
target_link_libraries(game PRIVATE bcrypt)


target_include_directories(game PUBLIC include)
target_include_directories(game PUBLIC include/utils)
target_include_directories(game PUBLIC include/macros)
target_include_directories(game PUBLIC include/debug)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

install(TARGETS game RUNTIME DESTINATION bin)
install(FILES "${CMAKE_SOURCE_DIR}/libs/d3d9.dll" DESTINATION bin)

include(CTest)
enable_testing()

add_executable(smoketest src/test/test.cpp)

add_test(NAME "SmokeTest" COMMAND smoketest)


