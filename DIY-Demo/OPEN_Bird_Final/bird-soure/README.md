# Eclipse project for SquareLine Studio and LVGL

This project can be used to build and run C/C++ UI code exported from SquareLine Studio.

## Get started
- Install a compiler and related tools if you don't have them yet:
  - On Windows [MinGW](https://www.mingw-w64.org/) can be a good choice
  - On Linux type in the Terminal: `sudo apt-get install gcc g++ gdb build-essential`
- Install SDL (required to simulate a display)
  - On Windows see for example [this guide](https://www.caveofprogramming.com/c-for-complete-beginners/setting-up-sdl-windows.html)
  - On Linux `sudo apt-get install libsdl2-dev`
- Download and Install Eclipse CDT from [http://www.eclipse.org/cdt/](http://www.eclipse.org/cdt/)

## Add the UI files
In SquareLine Studio click `Export->Export Files` and select the `ui` folder of this project

## Open and build the project
- Start Eclipse
- Select the parent folder of the exported project as "Workspace"
- Select `File->Import->General->Exisiting project into Workspace` click "Next" and browse the project
- Build the project with `Project->Build`
- Run the project with `Run->Run`.


