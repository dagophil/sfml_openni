cmake_minimum_required(VERSION 2.8)

project(proj)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/config)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Fix for Qt Creator to list the hxx files.
file(GLOB HEADERS "include/*.hxx")
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

# NITE
find_package(NITE REQUIRED)
include_directories(${NITE_INCLUDE_DIRS})

# The executables
add_executable(proj proj.cxx tinyxml2.cpp)
target_link_libraries(proj
    ${SFML_LIBRARIES}
    ${OPENNI_LIBRARIES}
    ${NITE_LIBRARIES}
)
add_custom_command(TARGET proj PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/fonts $<TARGET_FILE_DIR:proj>/fonts)
                   
# Test menu
add_executable(testmenu testmenu.cxx tinyxml2.cpp)
target_link_libraries(testmenu
    ${SFML_LIBRARIES}
    ${TINYXML2_LIBRARIES}
)
add_custom_command(TARGET testmenu PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/fonts $<TARGET_FILE_DIR:testmenu>/fonts)
add_custom_command(TARGET testmenu PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/images $<TARGET_FILE_DIR:testmenu>/images)


