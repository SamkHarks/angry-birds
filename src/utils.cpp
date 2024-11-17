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

    std::string getAssetsPath() {
        std::string execPath = getExecutablePath();
        // Navigate up two directories to reach the root directory
        std::string assetsPath = execPath + "/../../assets";
        return assetsPath;
    }

    float B2ToSf(float b2Coord) {
        return b2Coord * SCALE;
    }

    float SfToB2(float sfCoord) {
        return sfCoord / SCALE;
    }

    sf::Vector2f B2ToSf(const b2Vec2& b2Vector) {
        return sf::Vector2f(B2ToSf(b2Vector.x), B2ToSf(b2Vector.y));
    }

    b2Vec2 SfToB2(const sf::Vector2f& sfVector) {
        return b2Vec2(SfToB2(sfVector.x), SfToB2(sfVector.y));
    }

    sf::Vector2f B2ToSfCoords(const b2Vec2& b2Vector) {
        return sf::Vector2f(B2ToSf(b2Vector.x), VIEW.getHeight() - ( B2ToSf(b2Vector.y) ));
    }

    b2Vec2 SfToB2Coords(const sf::Vector2f& sfVector) {
        return b2Vec2(SfToB2(sfVector.x), SfToB2(VIEW.getHeight() - sfVector.y));
    }

    float RadiansToDegrees(const float radians) {
        return radians * 180 / b2_pi;
    }

    float DegreesToRadians(const float degrees) {
        return degrees * b2_pi / 180;
    }

    float getDirection(const sf::Vector2f& difference) {
        float direction = RadiansToDegrees(atan2(difference.y, difference.x));
        if (difference.x < 0) {
            direction += 180;
        }
        return direction;
    }

    float getScaleFactor(float originalWidth, float originalHeight, float targetWidth, float targetHeight) {
        float scaleFactorX = targetWidth / originalWidth;
        float scaleFactorY = targetHeight / originalHeight;
        float scaleFactor;
        if (scaleFactorX * originalHeight >= targetHeight) {
            scaleFactor = scaleFactorX;
        } else {
            scaleFactor = scaleFactorY;
        }
        return scaleFactor;
    }

    
    // Function to get the global coordinates of the 4 corners of a sprite
    std::array<sf::Vector2f, 4> getSpriteCorners(const sf::Sprite& sprite) {
        sf::FloatRect bounds = sprite.getLocalBounds();  // Get the untransformed local bounds (width and height)

        // Compute the 4 corners in local space, relative to the origin
        sf::Vector2f topLeft(0.f, 0.f);
        sf::Vector2f topRight(bounds.width, 0.f);
        sf::Vector2f bottomLeft(0.f, bounds.height);
        sf::Vector2f bottomRight(bounds.width, bounds.height);

        // Transform these points to global space using the sprite's transform
        sf::Transform transform = sprite.getTransform();
        std::array<sf::Vector2f, 4> corners = {
            transform.transformPoint(topLeft),
            transform.transformPoint(topRight),
            transform.transformPoint(bottomLeft),
            transform.transformPoint(bottomRight)
        };

        return corners;
    }


    // Helper function to project a point onto an axis
    float projectOntoAxis(const sf::Vector2f& point, const sf::Vector2f& axis) {
        float dotProduct = (point.x * axis.x) + (point.y * axis.y);
        float axisLengthSquared = (axis.x * axis.x) + (axis.y * axis.y);
        return dotProduct / std::sqrt(axisLengthSquared);
    }

    // Helper function to find the minimum and maximum projection of a box on an axis
    std::pair<float, float> getMinMaxProjection(const std::array<sf::Vector2f, 4>& corners, const sf::Vector2f& axis) {
        float min = projectOntoAxis(corners[0], axis);
        float max = min;

        for (const auto& corner : corners) {
            float projection = projectOntoAxis(corner, axis);
            if (projection < min) min = projection;
            if (projection > max) max = projection;
        }

        return { min, max };
    }

    // SAT collision detection between two sprites
    bool checkOBBCollision(const sf::Sprite& spriteA, const sf::Sprite& spriteB) {
        // Get the corners of both sprites
        auto cornersA = getSpriteCorners(spriteA);
        auto cornersB = getSpriteCorners(spriteB);

        // Define potential separating axes (normals of the edges)
        std::array<sf::Vector2f, 4> axes = {
            sf::Vector2f(cornersA[1].x - cornersA[0].x, cornersA[1].y - cornersA[0].y),  // Edge 1 of sprite A
            sf::Vector2f(cornersA[1].x - cornersA[3].x, cornersA[1].y - cornersA[3].y),  // Edge 2 of sprite A
            sf::Vector2f(cornersB[1].x - cornersB[0].x, cornersB[1].y - cornersB[0].y),  // Edge 1 of sprite B
            sf::Vector2f(cornersB[1].x - cornersB[3].x, cornersB[1].y - cornersB[3].y)   // Edge 2 of sprite B
        };

        // For each axis, project the corners of both sprites and check for overlap
        for (const auto& axis : axes) {
            // Normalize the axis
            float length = std::sqrt(axis.x * axis.x + axis.y * axis.y);
            sf::Vector2f normalizedAxis = sf::Vector2f(axis.x / length, axis.y / length);

            // Get the min/max projections of both sprites onto this axis
            auto [minA, maxA] = getMinMaxProjection(cornersA, normalizedAxis);
            auto [minB, maxB] = getMinMaxProjection(cornersB, normalizedAxis);

            // If there is no overlap between the projections, no collision
            if (maxA < minB || maxB < minA) {
                return false;  // Separating axis found, so no collision
            }
        }

        // If we didn't find a separating axis, then the OBBs must be colliding
        return true;
    }

    int countFilesInDirectory() {
        // Get the path of the folder containing the levels
        std::string folderPath = utils::getExecutablePath() + "/assets/levels/";
        int fileCount = 0;

        // Iterate through the directory and count the regular files, which are level<i>.json, where i = 1,2,3,...
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json" && entry.path().filename().string().find("level") != std::string::npos) {
                fileCount++;
            }
        }

        return fileCount;
    }
}