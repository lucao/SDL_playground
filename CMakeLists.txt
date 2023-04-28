cmake_minimum_required(VERSION 3.8)

project(game VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG -DDEBUG)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(CUSTOM_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${CUSTOM_INCLUDE_DIR})

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)
set(IMGUI_HEADERS ${IMGUI_DIR}/imgui.h 
${IMGUI_DIR}/imconfig.h 
${IMGUI_DIR}/imgui_internal.h 
${IMGUI_DIR}/imstb_rectpack.h
${IMGUI_DIR}/imstb_textedit.h
${IMGUI_DIR}/imstb_truetype.h
${IMGUI_DIR}/backends/imgui_impl_win32.h,
${IMGUI_DIR}/backends/imgui_impl_dx11.h)

set(IMGUI_SOURCES ${IMGUI_DIR}/imgui.cpp 
${IMGUI_DIR}/backends/imgui_impl_win32.cpp
${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
${IMGUI_DIR}/imgui_draw.cpp
${IMGUI_DIR}/imgui_tables.cpp
${IMGUI_DIR}/imgui_widgets.cpp)

set(CUSTOM_CLASSES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/custom_classes/src)
set(CUSTOM_CLASSES ${CUSTOM_CLASSES_DIR}/CustomGameCharacters.cpp 
${CUSTOM_CLASSES_DIR}/CustomScreen.cpp 
${CUSTOM_CLASSES_DIR}/CustomSDLObject.cpp 
${CUSTOM_CLASSES_DIR}/Stage.cpp 
${CUSTOM_CLASSES_DIR}/World.cpp) 

set(CMAKE_MINGW_DIR "C:/msys64/mingw64")

add_executable(main WIN32 src/main.cpp ${CUSTOM_CLASSES} ${IMGUI_SOURCES})

add_executable(unitTests src/test/Tests.cpp ${CUSTOM_CLASSES})

target_include_directories(unitTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/macros)
target_include_directories(unitTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(unitTests PUBLIC ${CMAKE_MINGW_DIR}/include)
target_include_directories(unitTests PRIVATE ${CMAKE_MINGW_DIR}/include/SDL2)
target_include_directories(unitTests PRIVATE ${IMGUI_DIR})
target_include_directories(unitTests PRIVATE ${IMGUI_DIR}/backends)



target_link_libraries(unitTests PRIVATE SDL2)
target_link_libraries(unitTests PRIVATE SDL2main)
target_link_libraries(unitTests PRIVATE SDL2_image)

add_test(NAME unitTests COMMAND unitTests.exe)

target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/macros)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/SDL2)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/cute)
target_include_directories(main PUBLIC ${CMAKE_MINGW_DIR}/include)
target_include_directories(main PRIVATE ${IMGUI_DIR})
target_include_directories(main PRIVATE ${IMGUI_DIR}/backends)

target_link_directories(main PUBLIC ${CMAKE_MINGW_DIR}/lib)
target_link_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/lib/)



if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(main PRIVATE SDL2::SDL2main)
endif()
# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(main PRIVATE SDL2)
target_link_libraries(main PRIVATE SDL2main)
target_link_libraries(main PRIVATE SDL2_image)

target_link_libraries(main PUBLIC d3d11)
target_link_libraries(main PUBLIC d3dcompiler)
target_link_libraries(main PUBLIC dwmapi)

#include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
#target_link_libraries(game ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

