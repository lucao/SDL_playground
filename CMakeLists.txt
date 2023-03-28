cmake_minimum_required(VERSION 3.7)

project(game VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(SOURCE_PREFIX src/)
set(SOURCES ${SOURCE_PREFIX}main.cpp)
set(SOURCES ${SOURCES} ${SOURCE_PREFIX}custom_classes/CustomSDLObject.h ${SOURCE_PREFIX}custom_classes/CustomGameCharacters.h ${SOURCE_PREFIX}custom_classes/CustomPosition.h)
set(SOURCES ${SOURCES} ${SOURCE_PREFIX}custom_classes/src/CustomSDLObject.cpp ${SOURCE_PREFIX}custom_classes/src/CustomGameCharacters.cpp ${SOURCE_PREFIX}custom_classes/src/CustomPosition.cpp)

set(PNG_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(PNG_PNG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/sdl2-image/external/zlib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/sdl2-image/external/jpeg)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/sdl2-image/external/libpng)

# 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
# 1. Look for a SDL2 package, 2. Look for the SDL2maincomponent and 3. DO NOT fail when SDL2main is not available
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

set(CMAKE_MODULE_PATH ${CMAKE_ORIGINAL_MODULE_PATH})
list(APPEND CMAKE_MODULE_PATH ${SDL2_image_DIR})

find_package(SDL2_image REQUIRED)

add_executable(game WIN32 ${SOURCES})

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(game PRIVATE SDL2::SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(game PRIVATE SDL2::SDL2)
target_link_libraries(game PRIVATE SDL2_image)

#include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
#target_link_libraries(game ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

