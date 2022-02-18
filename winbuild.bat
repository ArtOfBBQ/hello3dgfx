echo build script for windows...

echo delete previous build...
del /q build\windows

echo create output folder...
mkdir build\windows

echo compile program...
gcc src\windows\main.c src\shared\window_size.c src\shared\box.c src\shared\software_renderer.c -o build\windows\hello3dgfx.exe

echo running program...
build\windows\hello3dgfx.exe

