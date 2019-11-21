set -x

# Create build directory
mkdir build
cd build
BUILD_CONFIG=Release

# CMake
cmake .. \
	-G "Ninja" \
	-DCMAKE_BUILD_TYPE=$BUILD_CONFIG \
	-DCMAKE_PREFIX_PATH:PATH="${PREFIX}" \
	-DCMAKE_INSTALL_PREFIX:PATH="${PREFIX}" \
	-DCMAKE_INSTALL_RPATH:PATH="${PREFIX}/lib" \
	-DBOOST_ROOT:PATH="${PREFIX}" \
	-DBUILD_SHARED_LIBS:BOOL=OFF \
	-DENABLE_TESTING:BOOL=ON

# Compile and install
ninja install -v

# Run tests
ctest -V
