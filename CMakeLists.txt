cmake_minimum_required(VERSION 3.8)

project(game VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(CUSTOM_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/include)

include_directories(${CUSTOM_INCLUDE_DIR})


set(CUSTOM_CLASSES src/custom_classes/src/CustomGameCharacters.cpp src/custom_classes/src/CustomScreen.cpp src/custom_classes/src/CustomSDLObject.cpp src/custom_classes/src/Stage.cpp src/custom_classes/src/World.cpp) 

#SET(CMAKE_FIND_PACKAGE_REDIRECTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")
#find_package(SDL2 CONFIG REQUIRED)
#find_package(SDL2_image CONFIG REQUIRED)

#message(CMAKE_CURRENT_LIST_DIR="${${CMAKE_CURRENT_LIST_DIR}}")
set(CMAKE_MINGW_DIR "C:/msys64/mingw64")

add_executable(main WIN32 src/main.cpp ${CUSTOM_CLASSES})

target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/macros)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(main PUBLIC ${CMAKE_MINGW_DIR}/include)
target_include_directories(main PRIVATE ${CMAKE_MINGW_DIR}/include/SDL2)
target_link_directories(main PUBLIC ${CMAKE_MINGW_DIR}/lib)


if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(main PRIVATE SDL2::SDL2main)
endif()
# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(main PRIVATE SDL2)
target_link_libraries(main PRIVATE SDL2main)
target_link_libraries(main PRIVATE SDL2_image)

#include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
#target_link_libraries(game ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

