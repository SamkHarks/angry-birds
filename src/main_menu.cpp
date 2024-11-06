#include "main_menu.hpp"
#include <vector>
#include "utils.hpp"
#include "resource_manager.hpp"


MainMenu::MainMenu() : Menu(Menu::Type::MAIN, 3) {
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
        menuItems_[i].setPosition(VIEW.getCenter().x, (VIEW.getCenter().y - 100) + i * 100);
        if (i == selectedItem_) {
            menuItems_[i].setFillColor(LIME_GREEN);
            menuItems_[i].setScale(1.1f, 1.1f);
        }
    }

    setTitle("AngryBirds", 680.f, 340);
    backgroundMusic_.play();
};

void MainMenu::handleResize() {
    Menu::handleResize();
    for (int i = 0; i < buttonAmount_; ++i) {
        menuItems_[i].setPosition(VIEW.getCenter().x, (VIEW.getCenter().y - 100) + i * 100);
    }
    title_.clear();
    setTitle("AngryBirds", 680.f, 340);
};

void MainMenu::draw(sf::RenderWindow& window) const {
    window.draw(background_);
    window.draw(woodenSign_);
    // Draw the title characters
    for (const auto& character : title_) {
        window.draw(character);
    }
    // Draw menu items
    for (auto menuItem : menuItems_) {
        window.draw(menuItem);
    }
};
