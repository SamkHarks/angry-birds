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

struct View {

    int getWidth() const {
        return WIDTH;
    }

    int getHeight() const {
        return HEIGHT;
    }

    const sf::Vector2f& getCenter() const {
        return CENTER;
    }

    void setSize(int width, int height) {
        WIDTH = width;
        HEIGHT = height;
        CENTER = sf::Vector2f(WIDTH / 2.f, HEIGHT / 2.f);
    }
    private: 
        int WIDTH = 1500;
        int HEIGHT = 900;
        sf::Vector2f CENTER = sf::Vector2f(WIDTH / 2, HEIGHT / 2);
};

inline View VIEW;
const int WORLD_WIDTH = VIEW.getWidth() * 2;
const int WORLD_HEIGHT = VIEW.getHeight() * 1.8f;
const float SCALE = 100.f;
const int FRAME_RATE = 60.f;
const float TIME_STEP = (1.0f / FRAME_RATE) / 2;
const int VELOCITY_ITERATIONS = 6;
const int POSITION_ITERATIONS = 2;
const b2Vec2 BIRD_INITIAL_POSITION = b2Vec2(3.f, 1.3f);
const float IS_SETTLED_THRESHOLD = 0.01f;
const sf::Color LIME_GREEN(100, 255, 0);
const int PLAYER_INDEX_START = 5;
const b2Vec2 GROUND_DIMENSIONS = b2Vec2(VIEW.getWidth() / SCALE, 50.f / SCALE); // half width and half height of the ground

std::istream &operator>>(std::istream &input, b2Vec2 &vector);

std::istream &operator>>(std::istream &input, b2BodyType &type);

namespace utils
{
    std::string getExecutablePath();

    std::string getAssetsPath();

    template <typename T>
    bool loadFromFile(T& object, const std::string& FilePath) {
        std::string executablePath = getExecutablePath();
        return object.loadFromFile(executablePath + FilePath);
    }
    float B2ToSf(float b2Coord);

    float SfToB2(float sfCoord);

    sf::Vector2f B2ToSf(const b2Vec2& b2Vector);

    b2Vec2 SfToB2(const sf::Vector2f& sfVector);

    sf::Vector2f B2ToSfCoords(const b2Vec2& b2Vector);

    b2Vec2 SfToB2Coords(const sf::Vector2f& sfVector);

    float RadiansToDegrees(const float radians);

    float DegreesToRadians(const float degrees);

    float getDirection(const sf::Vector2f& difference);
}

#endif // UTILS_HPP