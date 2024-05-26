# Visual Studio Code project for SquareLine Studio and LVGL

This project can be used to build and run C/C++ UI code exported from SquareLine Studio.

## Get started
- Install a compiler and related tools if you don't have them yet:
  - On Windows [MinGW](https://www.mingw-w64.org/) can be a good choice
  - On Linux type in the Terminal: `sudo apt-get install gcc g++ gdb build-essential`
  - On MacOS install latest Xcode
- Install SDL (required to simulate a display)
  - On Windows see for example [this guide](https://www.caveofprogramming.com/c-for-complete-beginners/setting-up-sdl-windows.html)
  - On Linux `sudo apt-get install libsdl2-dev`
  - On MacOS install for example via Homebrew: `brew install sdl2` (To install Homebrew follow [the guide here](https://brew.sh/))
- Download and Install Visual Studio Code from [https://code.visualstudio.com/download])

## Add the UI files
In SquareLine Studio click `Export->Export Files` and select the `ui` folder of this project

## Open and build the project
- Start Visual Studio Code
- Select `File->Open Folder...` and select the exported project's folder
- If asked, allow it to download and install the CMake extension.
- Open the `main.c` file, and if asked allow it to download and install the C/C++ extension.
- Select the build toolkit on the bottom toolbar. (For example `GCC` on Linux/MinGW and `Clang` on MacOS)
- Configure the CMake project with the `CMake: ...` button on the bottom toolbar. (Select preferred release type)
- Build the project with the `Build` button on the bottom toolbar.
- Run the project with `Launch` button on the bottom toolbar.


