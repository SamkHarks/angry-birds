#include "menu.hpp"
#include "utils.hpp"
#include <iostream>

Menu::Menu(Type type) : type_(type) {
    if(!utils::loadFromFile(font_, "/assets/fonts/BerkshireSwash-Regular.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        std::exit(1);
    }
    if(!utils::loadFromFile(background_image_, "/assets/images/background.jpg")) {
        std::cerr << "Failed to load image" << std::endl;
        std::exit(1);
    }
    background_.setSize(sf::Vector2f(VIEW_WIDTH, VIEW_HEIGHT));
    background_.setTexture(&background_image_);
    background_.setPosition(0,0);
}

const Menu::Type Menu::getType() const {
    return type_;
}