cmake -S . -B ./build -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DCMAKE_CXX_COMPILER=cl -DCMAKE_C_COMPILER=cl
cmake --build ./build
build\NoRo.exe
