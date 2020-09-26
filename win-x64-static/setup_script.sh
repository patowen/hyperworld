#!/bin/sh

set -e

mkdir -p dependency-workspace
mkdir -p dependencies

cd dependency-workspace

# DOWNLOAD DEPENDENCIES
wget http://prdownloads.sourceforge.net/libpng/libpng-1.6.37.tar.xz?download -O libpng-1.6.37.tar.xz
echo "505e70834d35383537b6491e7ae8641f1a4bed1876dbfe361201fc80868d88ca libpng-1.6.37.tar.xz" | sha256sum --check

wget https://zlib.net/zlib-1.2.11.tar.xz
echo "4ff941449631ace0d4d203e3483be9dbc9da454084111f97ea0a2114e19bf066 zlib-1.2.11.tar.xz" | sha256sum --check

wget https://github.com/glfw/glfw/releases/download/3.3.2/glfw-3.3.2.zip
echo "08a33a512f29d7dbf78eab39bd7858576adcc95228c9efe8e4bc5f0f3261efc7 glfw-3.3.2.zip" | sha256sum --check

# On MSYS2, "tar -xJf" hangs for an unknown reason. This is a workaround.
xz --decompress --stdout zlib-1.2.11.tar.xz | tar -xf -
xz --decompress --stdout libpng-1.6.37.tar.xz | tar -xf -
unzip -q glfw-3.3.2.zip

# INSTALL ZLIB
cd zlib-1.2.11
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=../../../dependencies
mingw32-make zlibstatic
# Manual install to only install static library (https://github.com/madler/zlib/issues/359)
mkdir -p ../../../dependencies/lib
cp libzlibstatic.a ../../../dependencies/lib
mkdir -p ../../../dependencies/include
cp zconf.h ../../../dependencies/include
cp ../zlib.h ../../../dependencies/include
cd ../..

# INSTALL LIBPNG
cd libpng-1.6.37
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DPNG_BUILD_ZLIB=../zlib-build -DPNG_SHARED=OFF -DCMAKE_INSTALL_PREFIX=../../../dependencies
mingw32-make
mingw32-make install
cd ../..

# INSTALL GLFW
cd glfw-3.3.2
mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DCMAKE_INSTALL_PREFIX=../../../dependencies
mingw32-make
mingw32-make install
cd ../..

# BUILD HYPERWORLD
cd ../..
mkdir build
cd build
cmake ../src -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=../win-x64-static/dependencies
mingw32-make
cp -r ../src/textures .
