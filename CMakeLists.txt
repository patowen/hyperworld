cmake_minimum_required(VERSION 3.16)
#set(CMAKE_FIND_DEBUG_MODE 1)
set(MSVC_RUNTIME_LIBRARY MultiThreaded)
project(Hyperworld)

find_package(OpenGL REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(PNG REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_definitions(${PNG_DEFINITIONS})
if (MSVC)
  add_definitions(/bigobj)
endif (MSVC)

add_executable(Hyperworld main.cpp TextureLoader.cpp glad.c)
include(InstallRequiredSystemLibraries)
target_include_directories(Hyperworld PRIVATE ${PNG_INCLUDE_DIRS})
target_link_libraries(Hyperworld PRIVATE OpenGL::GL Eigen3::Eigen ${PNG_LIBRARIES} glfw)
