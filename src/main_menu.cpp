#include "main_menu.hpp"
#include <vector>
#include "utils.hpp"
#include "resource_manager.hpp"


MainMenu::MainMenu() : Menu(Menu::Type::MAIN, 3) {
    // Load wooden sign texture
    signImage_ = ResourceManager::getInstance().getTexture("/assets/images/wooden_sign.png");
    woodenSign_.setSize(sf::Vector2f(860, 860));
    woodenSign_.setTexture(&signImage_);
    woodenSign_.setOrigin(woodenSign_.getGlobalBounds().width / 2, woodenSign_.getGlobalBounds().height / 2);
    woodenSign_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y - 30);
    // Create menu items
    std::vector<std::string> button_texts = { "Play", "Settings", "Exit" };
    for (int i = 0; i < buttonAmount_; ++i) {
        menuItems_[i].setFont(font_);
        menuItems_[i].setString(button_texts[i]);
        menuItems_[i].setFillColor(sf::Color::White);
        menuItems_[i].setOutlineColor(sf::Color::Black);
        menuItems_[i].setOutlineThickness(5);
        menuItems_[i].setCharacterSize(80);
        // center the text
        sf::FloatRect textBounds = menuItems_[i].getGlobalBounds();
        menuItems_[i].setOrigin(textBounds.width / 2, textBounds.height / 2);
        menuItems_[i].setPosition(SCREEN_CENTER.x, 350 + i * 100);
        if (i == selectedItem_) {
            menuItems_[i].setFillColor(LIME_GREEN);
            menuItems_[i].setScale(1.1f, 1.1f);
        }
    }
};

void MainMenu::draw(sf::RenderWindow& window) const {
    window.draw(background_);
    window.draw(woodenSign_);
    for (auto menuItem : menuItems_)
    {
        window.draw(menuItem);
    }
};