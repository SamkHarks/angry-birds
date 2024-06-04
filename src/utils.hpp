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

const int VIEW_WIDTH = 1024;
const int VIEW_HEIGHT = 720;
const float SCALE = 100.0f;

std::istream &operator>>(std::istream &input, b2Vec2 &vector);

std::istream &operator>>(std::istream &input, b2BodyType &type);

namespace utils
{
    std::string getExecutablePath();

    template <typename T>
    bool loadFromFile(T& object, const std::string& FilePath) {
        std::string executablePath = getExecutablePath();
        return object.loadFromFile(executablePath + FilePath);
    }
    sf::Vector2f B2ToSfCoords(const b2Vec2 b2_vector);

    b2Vec2 SfToB2Coords(const sf::Vector2f sf_vector);

    float RadiansToDegrees(const float radians);

    float DegreesToRadians(const float degrees);
}

#endif // UTILS_HPP