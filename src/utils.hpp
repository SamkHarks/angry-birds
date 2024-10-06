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

const int VIEW_WIDTH = 1500;
const int VIEW_HEIGHT = 900;
const float SCALE = 100.f;
const int FRAME_RATE = 60.f;
const float TIME_STEP = (1.0f / FRAME_RATE) / 2;
const int VELOCITY_ITERATIONS = 6;
const int POSITION_ITERATIONS = 2;
const b2Vec2 BIRD_INITIAL_POSITION = b2Vec2(3.f, 1.3f);
const float IS_SETTLED_THRESHOLD = 0.01f;
const sf::Vector2f SCREEN_CENTER(VIEW_WIDTH / 2.f, VIEW_HEIGHT / 2.f);
const sf::Color LIME_GREEN(100, 255, 0);

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
    float B2ToSf(float b2_coord);

    float SfToB2(float sf_coord);

    sf::Vector2f B2ToSf(const b2Vec2& b2_vector);

    b2Vec2 SfToB2(const sf::Vector2f& sf_vector);

    sf::Vector2f B2ToSfCoords(const b2Vec2& b2_vector);

    b2Vec2 SfToB2Coords(const sf::Vector2f& sf_vector);

    float RadiansToDegrees(const float radians);

    float DegreesToRadians(const float degrees);

    float getDirection(const sf::Vector2f& difference);
}

#endif // UTILS_HPP