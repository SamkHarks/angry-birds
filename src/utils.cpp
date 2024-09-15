#include "utils.hpp"

std::istream &operator>>(std::istream &input, b2Vec2 &vector) {
    float x,y;
    char comma; // to store the comma
    input.ignore(); // ignore the opening bracket "("
    input >> x >> comma >> y;
    if (input) {
        vector.Set(x, y);
    } else {
        input.setstate(std::ios::failbit);
    }
    input.ignore(); // ignore the closing bracket ")"

    return input;
}

std::istream &operator>>(std::istream &input, b2BodyType &type) {
    int bodyType;
    input >> bodyType;
    if (bodyType == 0) {
        type = b2_staticBody;
    } else if (bodyType == 1) {
        type = b2_kinematicBody;
    } else if (bodyType == 2) {
        type = b2_dynamicBody;
    } else {
        input.setstate(std::ios::failbit);
    }
    return input;
}

std::istream &operator>>(std::istream &input, bool awake) {
    int isAwake;
    input >> isAwake;
    if (isAwake == 0) {
        awake = false;
    } else if (isAwake == 1) {
        awake = true;
    } else {
        input.setstate(std::ios::failbit);
    }
    return input;
}
namespace utils
{
    std::string getExecutablePath() {
        std::string path;
        #ifdef _WIN32
            char result[MAX_PATH];
            DWORD count = GetModuleFileName(NULL, result, MAX_PATH);
            path = std::string(result, (count > 0) ? count : 0);
        #elif __APPLE__
            char result[PATH_MAX];
            uint32_t count = sizeof(result);
            if (_NSGetExecutablePath(result, &count) == 0)
                path = std::string(result);
        #else
            char result[PATH_MAX];
            ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
            path = std::string(result, (count > 0) ? count : 0);
        #endif
        return path.substr(0, path.find_last_of("/\\"));
    }



    sf::Vector2f B2ToSfCoords(const b2Vec2 b2_vector) {
        return sf::Vector2f(b2_vector.x * SCALE, VIEW_HEIGHT - ( b2_vector.y * SCALE ));
    }

    b2Vec2 SfToB2Coords(const sf::Vector2f sf_vector) {
        return b2Vec2(sf_vector.x / SCALE, ( VIEW_HEIGHT - sf_vector.y )/ SCALE);
    }

    float RadiansToDegrees(const float radians) {
        return radians * 180 / b2_pi;
    }

    float DegreesToRadians(const float degrees) {
        return degrees * b2_pi / 180;
    }
}