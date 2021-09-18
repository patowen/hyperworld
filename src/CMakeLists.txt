cmake_minimum_required(VERSION 3.16)

project(Hyperworld)

# Use C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenGL is used for all 3D rendering
# https://cmake.org/cmake/help/latest/module/FindOpenGL.html
find_package(OpenGL REQUIRED)

# Eigen is used to help with math involving vectors. Instructions on how to include Eigen on CMake projects are in Eigen's
# documentation, not in CMake's documentation like some other libraries.
# https://eigen.tuxfamily.org/dox/TopicCMakeGuide.html
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

# The libpng library is needed to load PNG textures
# https://cmake.org/cmake/help/latest/module/FindPNG.html
find_package(PNG REQUIRED)

# GLFW provides a simple cross-platform way to set up an OpenGL rendering context and add keyboard and mouse controls.
# CMake instructions are in GLFW's build guide (With CMake and installed GLFW binaries)
# https://www.glfw.org/docs/3.3/build_guide.html#build_link_cmake_package
find_package(glfw3 3.3 REQUIRED)

add_definitions(${PNG_DEFINITIONS}) # See CMake documentation for FindPNG

# The Hyperworld program depends on all of these C/C++ source files. To reduce the amount of time spent linking, hopefully
# speeding up overall compile time, a separate cpp source file is not included for every class, but instead, the rule of
# thumb that has been used was to create a separate source file whenever there was some logic that would be nice to silo away,
# such as PNG loading or the use of unsupported Eigen libraries.
add_executable(
    Hyperworld
    main.cpp
    TextureLoader.cpp
    VectorMath.cpp
    glad.c
)

# WARNING: If this property is not set, MinGW32 will hang for a long time while linking and, after some time, produce
# some confusing output in the console every three or so seconds, mentioning an undefined reference to something related
# to Eigen. This can continue for up to an hour or so unless the process is cancelled with Ctrl-C. I believe it's
# because Eigen heavily depends on compiler optimizations due to the way it handles templates, so the dependencies of
# VectorMath.cpp are complicated enough that without compiler optimizations enabled, the maximum number of symbols that
# MinGW can handle is exceeded. Unfortunately, MinGW doesn't handle this error gracefully, so it can be very difficult to debug.
set_source_files_properties(VectorMath.cpp PROPERTIES COMPILE_FLAGS -O2)

target_include_directories(
    Hyperworld
    PRIVATE # As Hyperworld is not a library, the use of "PRIVATE" here was an arbitrary decision.
    ${PNG_INCLUDE_DIRS} # See CMake documentation for FindPNG
)

target_link_libraries(
    Hyperworld
    PRIVATE # As Hyperworld is not a library, the use of "PRIVATE" here was an arbitrary decision.
    OpenGL::GL # See CMake documentation for FindOpenGL
    Eigen3::Eigen # See the CMake documentation on Eigen's website
    ${PNG_LIBRARIES} # See CMake documentation for FindPNG
    glfw # See the build guide on GLFW's website
    -static-libstdc++ # This and "-static" help ensure that the resulting executable won't need extra MinGW-specific DLLs to run.
    -static
)
