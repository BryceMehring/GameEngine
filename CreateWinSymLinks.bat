@ECHO OFF
cd %~dp0

cd GameLauncher

mklink base.r ..\base.r 

cd ..

mkdir bin && cd bin && mkdir Debug && cd Debug

mklink glfw3.dll ..\..\..\glfw\lib-msvc110\glfw3.dll
mklink glew32.dll ..\..\..\glew\bin\Release\Win32\glew32.dll

cd ..\ && mkdir Release && cd Release

mklink /D textures ..\..\textures
mklink /D shaders ..\..\shaders
mklink glfw3.dll ..\..\..\glfw\lib-msvc110\glfw3.dll
mklink glew32.dll ..\..\..\glew\bin\Release\Win32\glew32.dll

pause