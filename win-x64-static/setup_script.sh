#!/bin/sh

set -e

# Define file names and their checksums
LIBPNG_NAME="libpng-1.6.37"
LIBPNG_FILE="$LIBPNG_NAME.tar.xz"
LIBPNG_CHECKSUM="505e70834d35383537b6491e7ae8641f1a4bed1876dbfe361201fc80868d88ca"

ZLIB_NAME="zlib-1.2.13"
ZLIB_FILE="$ZLIB_NAME.tar.gz"
ZLIB_CHECKSUM="b3a24de97a8fdbc835b9833169501030b8977031bcb54b3b3ac13740f846ab30"

GLFW_VERSION="3.3.2"
GLFW_NAME="glfw-$GLFW_VERSION"
GLFW_FILE="$GLFW_NAME.zip"
GLFW_CHECKSUM="08a33a512f29d7dbf78eab39bd7858576adcc95228c9efe8e4bc5f0f3261efc7"

mkdir -p dependency-workspace
mkdir -p dependencies

cd dependency-workspace

# DOWNLOAD DEPENDENCIES
if [ ! -f "$LIBPNG_FILE" ]; then
    wget http://prdownloads.sourceforge.net/libpng/$LIBPNG_FILE?download -O $LIBPNG_FILE
fi
echo "$LIBPNG_CHECKSUM $LIBPNG_FILE" | sha256sum --check

if [ ! -f "$ZLIB_FILE" ]; then
    wget https://zlib.net/fossils/$ZLIB_FILE
fi
echo "$ZLIB_CHECKSUM $ZLIB_FILE" | sha256sum --check

if [ ! -f "$GLFW_FILE" ]; then
    wget https://github.com/glfw/glfw/releases/download/$GLFW_VERSION/$GLFW_FILE
fi
echo "$GLFW_CHECKSUM $GLFW_FILE" | sha256sum --check

tar -zxvf $ZLIB_FILE
# On MSYS2, "tar -xJf" hangs for an unknown reason. This is a workaround.
xz --decompress --stdout $LIBPNG_FILE | tar -xf -
unzip -q $GLFW_FILE

# INSTALL ZLIB
if [ ! -d "$ZLIB_NAME/build" ]; then
	cd $ZLIB_NAME
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
fi

# INSTALL LIBPNG
if [ ! -d "$LIBPNG_NAME/build" ]; then
	cd $LIBPNG_NAME
	mkdir build
	cd build
	cmake .. -G "MinGW Makefiles" -DPNG_BUILD_ZLIB=../zlib-build -DPNG_SHARED=OFF -DCMAKE_INSTALL_PREFIX=../../../dependencies
	mingw32-make
	mingw32-make install
	cd ../..
fi

# INSTALL GLFW
if [ ! -d "$GLFW_NAME/build" ]; then
	cd $GLFW_NAME
	mkdir build
	cd build
	cmake .. -G "MinGW Makefiles" -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DCMAKE_INSTALL_PREFIX=../../../dependencies
	mingw32-make
	mingw32-make install
	cd ../..
fi

# BUILD HYPERWORLD
if [ -d "../../build" ]; then
	rm -r "../../build"
fi
cd ../..
mkdir build
cd build
cmake ../src -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=../win-x64-static/dependencies
mingw32-make
cp -r ../src/textures .
cp -r ../src/shaders .
