#include "menu.hpp"
#include "utils.hpp"
#include <iostream>
#include "resource_manager.hpp"

Menu::Menu(Type type, int buttonAmount) : type_(type), buttonAmount_(buttonAmount), menuItems_(buttonAmount) {
    font_ = ResourceManager::getInstance().getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
    backgroundImage_ = ResourceManager::getInstance().getTexture("/assets/images/background.jpg");
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

int Menu::getItemAtPosition(sf::Vector2f mousePosition) const {
    for (int i = 0; i < buttonAmount_; i++) {
        if (menuItems_[i].getGlobalBounds().contains(mousePosition)) {
            return i;
        }
    }
    return -1;
}

std::string Menu::handleMouseClick(sf::Vector2f mousePosition) {
    int clickedItem = getItemAtPosition(mousePosition);
    if (clickedItem == -1) {
        return "";
    }
    std::string buttonText = menuItems_[clickedItem].getString().toAnsiString();
    return buttonText;
};