cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_FIND_DEBUG_MODE 1)
project(Hyperworld)

find_package(OpenGL REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(PNG REQUIRED)
find_package(glfw3 3.3 REQUIRED)

add_definitions(${PNG_DEFINITIONS})

add_executable(Hyperworld main.cpp TextureLoader.cpp VectorMath.cpp glad.c)
set_source_files_properties(VectorMath.cpp PROPERTIES COMPILE_FLAGS -O2)
target_include_directories(Hyperworld PRIVATE ${PNG_INCLUDE_DIRS})
target_link_libraries(Hyperworld PRIVATE OpenGL::GL Eigen3::Eigen ${PNG_LIBRARIES} glfw -static-libstdc++ -static)
