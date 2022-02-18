APP_NAME="hello3dgfx"
PLATFORM="macos"

MAC_FRAMEWORKS="
    -framework AppKit 
    -framework MetalKit 
    -framework Metal"

echo "Building $APP_NAME for $PLATFORM..."

echo "deleting previous build(s)..."
rm -r -f build

echo "Creating build folder..."
mkdir build
mkdir build/$PLATFORM
mkdir build/$PLATFORM/$APP_NAME.app

echo "Creating metal library..."
xcrun -sdk macosx metal -gline-tables-only -MO -g -c "src/shared_apple/Shaders.metal" -o Shaders.air
xcrun -sdk macosx metal -c "src/shared_apple/shaders.metal" -o Shaders.air
xcrun -sdk macosx metallib Shaders.air -o build/$PLATFORM/$APP_NAME.app/Shaders.metallib
rm -r Shaders.air

echo "copy resources..."
cp resources/teddybear.obj build/$PLATFORM/$APP_NAME.app/teddybear.obj
cp resources/teapot.obj build/$PLATFORM/$APP_NAME.app/teapot.obj

echo "Compiling & linking $APP_NAME..."
clang -x objective-c -g -pedantic $MAC_FRAMEWORKS -objC src/$PLATFORM/main.mm src/shared_apple/gpu.m src/shared/box.c src/shared/software_renderer.c src/shared/window_size.c -o build/$PLATFORM/$APP_NAME.app/$APP_NAME

echo "Booting $APP_NAME"
(cd build/$PLATFORM/$APP_NAME.app && ./$APP_NAME)
# (cd build/$PLATFORM/$APP_NAME.app && gdb ./$APP_NAME)

