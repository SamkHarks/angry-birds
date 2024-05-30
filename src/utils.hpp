#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#ifdef _WIN32
    #include <windows.h>
#elif __APPLE__
    #include <mach-o/dyld.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

#ifndef MAX_PATH
#define MAX_PATH 4096
#endif

#include <SFML/Graphics/Texture.hpp>

std::string getExecutablePath();

template <typename T>
bool loadFromFile(T& object, const std::string& FilePath) {
    std::string executablePath = getExecutablePath();
    return object.loadFromFile(executablePath + FilePath);
}

#endif // UTILS_HPP