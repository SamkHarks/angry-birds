#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include "utils.hpp"

class ResourceManager {
public:
    // Static method to get the single instance of the class
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    // Methods to get resources
    sf::Font& getFont(const std::string& fontPath) {
        return getResource<sf::Font>(fontPath, fonts_);
    }

    sf::Texture& getTexture(const std::string& texturePath) {
        return getResource<sf::Texture>(texturePath, textures_);
    }
    // TODO: add method to get sound buffer once sound is implemented
    /*sf::SoundBuffer& getSoundBuffer(const std::string& soundPath) {
        return getResource<sf::SoundBuffer>(soundPath, soundBuffers_);
    }*/

private:
    // Private constructor to prevent instantiation
    ResourceManager() {}
    ~ResourceManager() {}

    // Delete copy constructor and assignment operator
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Template method to get or load a resource
    template <typename T>
    T& getResource(const std::string& path, std::unordered_map<std::string, std::unique_ptr<T>>& resourceMap) {
        auto it = resourceMap.find(path);
        if (it != resourceMap.end()) {
            return *(it->second);
        } else {
            std::unique_ptr<T> resource = std::make_unique<T>();
            if (!utils::loadFromFile(*resource, path)) {
                throw std::runtime_error("Failed to load resource: " + path);
            }
            T& resourceRef = *resource;
            resourceMap[path] = std::move(resource);
            return resourceRef;
        }
    }

    // Maps to store resources
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts_;
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures_;
    // TODO: add sound buffer map once sound is implemented
    //std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers_;
};

#endif // RESOURCE_MANAGER_HPP