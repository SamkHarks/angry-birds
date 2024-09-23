#include "menu.hpp"
#include "utils.hpp"
#include <iostream>

Menu::Menu(Type type, int buttonAmount) : type_(type), buttonAmount_(buttonAmount), menuItems_(buttonAmount) {
    if(!utils::loadFromFile(font_, "/assets/fonts/BerkshireSwash-Regular.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        std::exit(1);
    }
    if(!utils::loadFromFile(backgroundImage_, "/assets/images/background.jpg")) {
        std::cerr << "Failed to load image" << std::endl;
        std::exit(1);
    }
    background_.setSize(sf::Vector2f(VIEW_WIDTH, VIEW_HEIGHT));
    background_.setTexture(&backgroundImage_);
    background_.setPosition(0,0);
}

void Menu::draw(sf::RenderWindow &window) const {
    window.draw(background_);
    for (auto menuItem : menuItems_)
    {
        window.draw(menuItem);
    }
};

const Menu::Type Menu::getType() const {
    return type_;
}


void Menu::setSelectedItem(int nextItem) {
    updateItem(sf::Color::Black, 0);
    if (nextItem < 0) {
        selectedItem_ = buttonAmount_ - 1;
    } else if (nextItem >= buttonAmount_) {
        selectedItem_ = 0;
    } else {
        selectedItem_ = nextItem;
    }
    updateItem(sf::Color::White, 5);
}

const int Menu::getSelectedItem() const {
    return selectedItem_;
}

void Menu::updateItem(sf::Color color, int thickness) {
    menuItems_[selectedItem_].setOutlineColor(color);
    menuItems_[selectedItem_].setOutlineThickness(thickness);
}