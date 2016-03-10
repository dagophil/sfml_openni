cmake_minimum_required(VERSION 2.8)

project(proj)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/config)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Fix for Qt Creator to list the hxx files.
file(GLOB HEADERS "include/*.hxx" "include/*/*.hxx")
add_custom_target(headerfiles SOURCES ${HEADERS})

# Choose Release build by default
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Release)
endif()

# C++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  if("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 4.6)
    message(FATAL_ERROR "The gcc version is ${CMAKE_CXX_COMPILER_VERSION}, but you need at least 4.6.")
  endif()
endif()

# SFML
find_package(SFML REQUIRED system window graphics audio)
include_directories(${SFML_INCLUDE_DIR})

# OpenNI
find_package(OpenNI REQUIRED)
include_directories(${OPENNI_INCLUDE_DIRS})

## NITE
#find_package(NITE REQUIRED)
#include_directories(${NITE_INCLUDE_DIRS})

# The target for copying files.
add_custom_target(copy)
add_custom_command(TARGET copy PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/fonts ${CMAKE_BINARY_DIR}/fonts)
add_custom_command(TARGET copy PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/images ${CMAKE_BINARY_DIR}/images)
add_custom_command(TARGET copy PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/animations ${CMAKE_BINARY_DIR}/animations)
add_custom_command(TARGET copy PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/sounds ${CMAKE_BINARY_DIR}/sounds)
#add_custom_command(TARGET copy PRE_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy_directory
#                   ${CMAKE_SOURCE_DIR}/highscore ${CMAKE_BINARY_DIR}/highscore)
add_custom_command(TARGET copy PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/menu.xml ${CMAKE_BINARY_DIR}/menu.xml)
add_custom_command(TARGET copy PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/menu_small.xml ${CMAKE_BINARY_DIR}/menu_small.xml)

# Library: tinyxml2
add_library(tinyxml2 STATIC tinyxml2.cpp)

# Executable: The kinect menu.
add_executable(proj proj.cxx)
target_link_libraries(proj
    ${SFML_LIBRARIES}
    ${OPENNI_LIBRARIES}
#    ${NITE_LIBRARIES}
    tinyxml2
)
add_dependencies(proj copy)

# Executable: The test menu.
add_executable(testmenu testmenu.cxx)
target_link_libraries(testmenu
    ${SFML_LIBRARIES}
    tinyxml2
)
add_dependencies(testmenu copy)

# Executable: The game.
add_executable(hdm hdm.cxx)
target_link_libraries(hdm
    ${SFML_LIBRARIES}
)
add_dependencies(hdm copy)

# Executable: The game with kinect.
add_executable(hdm_kinect hdm_kinect.cxx)
target_link_libraries(hdm_kinect
    ${SFML_LIBRARIES}
    ${OPENNI_LIBRARIES}
)
add_dependencies(hdm_kinect copy)

# Executable: The widget test.
add_executable(test_widgets test_widgets.cxx)
target_link_libraries(test_widgets
    ${SFML_LIBRARIES}
)
add_dependencies(test_widgets copy)
