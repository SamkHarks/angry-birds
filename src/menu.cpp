#include "menu.hpp"
#include <iostream>

Menu::Menu(Type type) : type_(type) {
    if(!font_.loadFromFile("../../assets/fonts/BerkshireSwash-Regular.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        std::exit(1);
    }
    if(!background_image_.loadFromFile("../../assets/images/background.jpg")) {
        std::cerr << "Failed to load image" << std::endl;
        std::exit(1);
    }
    background_.setSize(sf::Vector2f(1024, 720));
    background_.setTexture(&background_image_);
    background_.setPosition(0,0);
}

const Menu::Type Menu::getType() const {
    return type_;
}