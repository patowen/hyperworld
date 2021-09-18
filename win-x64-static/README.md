# Static Windows x64 build instructions
## First-time build steps
1. If you haven't installed MSYS2 already, go to https://www.msys2.org/ and install MSYS2. As of the time these instructions were written, the download link is https://repo.msys2.org/distrib/x86_64/msys2-x86_64-20200903.exe. Follow the installation instructions on that website.
1. Open MSYS: `.\msys2_shell.cmd -msys` (or by finding "MSYS2 MSYS" in the start menu)
1. Run the following command to install required packages (pacman is short for package manager):

        pacman -Sy --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make unzip mingw-w64-x86_64-eigen3

1. Close MSYS.
1. Open MSYS MINGW64: `.\msys2_shell.cmd -mingw64` (or by finding "MSYS2 MinGW 64-bit" in the start menu)
1. Navigate to this directory (win-x64-static).
1. Run the setup script with `./setup_script.sh`. This script should not be run from another directory.
1. The executable should be able to found under the "build" directory in the repository. Launch it normally to try it out.
1. If it works, a window should open with a black screen. Press the "1" number key (the one on the main keyboard, not the numpad) to spawn a dodecahedron. You can then look around with the mouse and fly around with the WASD keys and mouse buttons.

## Rebuilding
After making changes, to rebuild, simply run `./mingw32-make` from the "build" directory with a MSYS MINGW64 console.

## Troubleshooting
- If cmake cannot find dependencies, it may be possible to see what's going on with `-DCMAKE_FIND_DEBUG_MODE=1`. Consider redirecting the verbose output by adding `2> verbose_output.txt` at the end.
- If trying to run the Hyperworld executable gives you an error message about a missing dll, please file a bug with the error message. The goal of the static build is to avoid these dependencies.
