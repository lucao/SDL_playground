cmake_minimum_required(VERSION 3.16)
project(game VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/boost EXCLUDE_FROM_ALL)
add_subdirectory(external/box2d EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL3 EXCLUDE_FROM_ALL)
add_subdirectory(external/SDL3_image EXCLUDE_FROM_ALL)
set(aom_DIR external/SDL3_image/external/aom_build)


add_library(dear_imgui
external/imgui/imgui.cpp
external/imgui/imgui_draw.cpp
external/imgui/imgui_tables.cpp
external/imgui/imgui_widgets.cpp
    
external/imgui/backends/imgui_impl_win32.cpp
external/imgui/backends/imgui_impl_dx11.cpp

external/imgui/imgui.h
external/imgui/imgui_internal.h
external/imgui/imconfig.h
external/imgui/imstb_truetype.h
external/imgui/imstb_textedit.h
external/imgui/imstb_rectpack.h)

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
src/engine/CustomTexture.cpp
)

add_executable(game src/main.cpp ${SOURCES})

link_directories(libs)

target_link_libraries(game PRIVATE d3d9)
target_link_libraries(game PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
target_link_libraries(game PRIVATE dear_imgui)
target_link_libraries(game PRIVATE box2d)
target_link_libraries(game PRIVATE bcrypt)
target_link_libraries(game PRIVATE Boost::hash2)
target_link_libraries(game PRIVATE Boost::uuid)

target_include_directories(game PUBLIC include)
target_include_directories(game PUBLIC include/utils)
target_include_directories(game PUBLIC include/macros)
target_include_directories(game PUBLIC include/debug)

target_include_directories(game PUBLIC external/box2d/include)
target_include_directories(game PUBLIC external/SDL3/include)
target_include_directories(game PUBLIC external/imgui)
target_include_directories(game PUBLIC external/imgui/backends)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

install(TARGETS game RUNTIME DESTINATION bin)
install(FILES "${CMAKE_SOURCE_DIR}/libs/d3d9.dll" DESTINATION bin)

include(CTest)
enable_testing()

add_executable(smoketest src/test/test.cpp)

add_test(NAME "SmokeTest" COMMAND smoketest)


