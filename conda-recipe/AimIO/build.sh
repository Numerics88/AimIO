set -x

# Create build directory
mkdir build
cd build
BUILD_CONFIG=Release

# Platform specifics
declare -a CMAKE_PLATFORM_FLAGS
case $(uname | tr '[:upper:]' '[:lower:]') in
  linux*)
    ;;
  darwin*)
		# Get the SDK
		CMAKE_PLATFORM_FLAGS+=(-DCMAKE_OSX_SYSROOT="${CONDA_BUILD_SYSROOT}")
    ;;
  *)
esac

# CMake
cmake .. \
	-G "Ninja" \
	-DCMAKE_BUILD_TYPE=$BUILD_CONFIG \
	-DCMAKE_PREFIX_PATH:PATH="${PREFIX}" \
	-DCMAKE_INSTALL_PREFIX:PATH="${PREFIX}" \
	-DCMAKE_INSTALL_RPATH:PATH="${PREFIX}/lib" \
	-DBOOST_ROOT:PATH="${PREFIX}" \
	-DBUILD_SHARED_LIBS:BOOL=OFF \
	-DENABLE_TESTING:BOOL=ON \
	"${CMAKE_PLATFORM_FLAGS[@]}"

# Compile and install
ninja install -v

# Run tests
ctest -V
