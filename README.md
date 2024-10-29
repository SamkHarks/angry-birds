
# Angry Birds Game

Welcome to my **Angry Birds Game**, a physics-based 2D game inspired by the original Angry Birds. This game is built using **SFML** (Simple and Fast Multimedia Library) for rendering and input handling, and **Box2D** for realistic physics simulation. Launch birds with a cannon to destroy structures and defeat the pigs!

## Features
- **2D Physics Simulation** powered by **Box2D**
- **Graphics Rendering** and **Input Handling** with **SFML**
- **JSON (nlohmann/json)** for data formatting when storing and reading data
- Main menu
- Settings - WIP
- Level end menu - WIP
- Pause menu - WIP
- Three birds with different powers - WIP
- Multiple levels with destructible environments - WIP
- High score - WIP
- Sound effects and background music - WIP
- Cross-platform support for Windows, Linux, and macOS

## Prerequisites

To build and run the game, ensure the following dependencies are installed:

- **CMake** (minimum version 3.16)
- A **C++17** compatible compiler (GCC, Clang, MSVC, etc.)

### Install Dependencies on Linux (Ubuntu Example)
For Ubuntu and other Debian-based systems, install SFML dependencies using:
```bash
sudo apt update
sudo apt install libxrandr-dev libxcursor-dev libudev-dev \
                 libfreetype-dev libopenal-dev libflac-dev \
                 libvorbis-dev libgl1-mesa-dev libegl1-mesa-dev
```

### Install Dependencies on Windows
- **CMake**: Download and install from [CMake official site](https://cmake.org/download/).
- **SFML** and **Box2D**: These are automatically fetched during the build process using CMake’s FetchContent.

## Build Instructions

1. **Clone the repository:**
   ```bash
   git clone <repository_url>
   cd angry-birds
   ```

2. **Configure and Build the Project:**
   ```bash
   cmake -B build
   cmake --build build
   ```

3. **Run the Game:**
   After building, you can run the game executable:
   ```bash
   ./build/bin/AngryBirds
   ```
   On Windows, the executable can be found in `build/bin/AngryBirds.exe`.

**OR Building with Visual Studio Code:** If you prefer using **VSCode**, you can take advantage of the **CMake Tools** extension:
- Open the project folder in VSCode.
- The extension will automatically detect the CMakeLists.txt file.
- Use the VSCode commands to configure (CMake: Configure), build (CMake: Build), and run (CMake: Run Without Debugging) the project directly within the editor.

### Notes for Windows Users:
- The build process automatically copies the necessary **OpenAL32.dll** to the output directory, ensuring audio support for SFML.

## How to Play
1. Aim the cannon to control the angle, and hold down the mouse button to adjust the power of the bird launch.
2. Use different bird powers to destroy pigs - WIP
3. Release to fire the bird at the structures.
4. Destroy all the pigs on each level to progress to the next. - WIP

The game features different levels with increasing difficulty and complex structures to destroy.

## Project Structure
- **src/**: Contains all the C++ source files.
- **assets/**: Stores the game's resources, including textures, sounds, and level data.
- **CMakeLists.txt**: The build configuration file using CMake.
- **build/**: Directory where compiled binaries are generated.

## SFML and Box2D Integration

- **SFML** is used for rendering, input handling, and window management.
- **Box2D** handles all the game physics, including bird movement, collision detection, and the destruction of structures..

## Cross-Platform Support

This project should support Windows, macOS, and Linux. CMake automatically configures and builds the project on all these platforms.

### Build Options
You can configure whether to build shared or static libraries by toggling the following option in `CMakeLists.txt`:
```cmake
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
```
Currently, static libraries are being used for both SFML and Box2D.

---

### Additional Resources
- [SFML Documentation](https://www.sfml-dev.org/documentation/2.6.1/)
- [Box2D Documentation](https://box2d.org/documentation/) (Note documentation is for version 3.x.x)
- [CMake Documentation](https://cmake.org/documentation/)

---
