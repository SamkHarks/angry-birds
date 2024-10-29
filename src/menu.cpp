#include "menu.hpp"
#include "utils.hpp"
#include <iostream>
#include "resource_manager.hpp"

Menu::Menu(Type type, int buttonAmount) : type_(type), buttonAmount_(buttonAmount), menuItems_(buttonAmount) {
    font_ = ResourceManager::getInstance().getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
    woodenSign_.setSize(sf::Vector2f(860, 860));
    woodenSign_.setTexture(&ResourceManager::getInstance().getTexture("/assets/images/wooden_sign.png"));
    woodenSign_.setOrigin(woodenSign_.getGlobalBounds().width / 2, woodenSign_.getGlobalBounds().height / 2);
    woodenSign_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y - 30);
    backgroundImage_ = ResourceManager::getInstance().getTexture("/assets/images/background.jpg");
    backgroundMusicBuffer_ = ResourceManager::getInstance().getSoundBuffer("/assets/sounds/menu_2.wav");
    background_.setSize(sf::Vector2f(VIEW_WIDTH, VIEW_HEIGHT));
    background_.setTexture(&backgroundImage_);
    background_.setPosition(0,0);
    backgroundMusic_.setBuffer(backgroundMusicBuffer_);
    backgroundMusic_.setVolume(10);
    backgroundMusic_.setLoop(true);
}

const Menu::Type Menu::getType() const {
    return type_;
}


void Menu::setSelectedItem(int nextItem) {
    updateItem(sf::Color::White, 1.f);
    if (nextItem < 0) {
        selectedItem_ = buttonAmount_ - 1;
    } else if (nextItem >= buttonAmount_) {
        selectedItem_ = 0;
    } else {
        selectedItem_ = nextItem;
    }
    updateItem(LIME_GREEN, 1.1f);
}

const int Menu::getSelectedItem() const {
    return selectedItem_;
}

void Menu::updateItem(sf::Color fillColor, float scale) {
    menuItems_[selectedItem_].setFillColor(fillColor);
    menuItems_[selectedItem_].setScale(scale, scale);
}

int Menu::getItemAtPosition(sf::Vector2f mousePosition) const {
    for (int i = 0; i < buttonAmount_; i++) {
        if (menuItems_[i].getGlobalBounds().contains(mousePosition)) {
            return i;
        }
    }
    return -1;
}

bool Menu::handleMouseClick(sf::Vector2f mousePosition) {
    return getItemAtPosition(mousePosition) >= 0;
};

void Menu::handleMouseMove(sf::Vector2f mousePosition) {
    int hoveredItem = getItemAtPosition(mousePosition);
    if (hoveredItem == -1) {
        return;
    }
    updateItem(sf::Color::White, 1.f);
    selectedItem_ = hoveredItem;
    updateItem(LIME_GREEN, 1.1f);
}

void Menu::updateMusic(sf::SoundSource::Status newStatus) {
    switch (newStatus) {
        case sf::SoundSource::Status::Playing:
            backgroundMusic_.play();
            break;
        case sf::SoundSource::Status::Paused:
            backgroundMusic_.pause();
            break;
        case sf::SoundSource::Status::Stopped:
            backgroundMusic_.stop();
            break;
        default:
            break;
    }
}
