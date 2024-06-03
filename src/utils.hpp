#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <box2d/box2d.h>
#include <iostream>
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

std::istream &operator>>(std::istream &input, b2Vec2 &vector);

std::istream &operator>>(std::istream &input, b2BodyType &type);

std::string getExecutablePath();

template <typename T>
bool loadFromFile(T& object, const std::string& FilePath) {
    std::string executablePath = getExecutablePath();
    return object.loadFromFile(executablePath + FilePath);
}

#endif // UTILS_HPP