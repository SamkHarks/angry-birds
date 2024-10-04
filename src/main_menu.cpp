#include "main_menu.hpp"
#include <vector>
#include "utils.hpp"


MainMenu::MainMenu() : Menu(Menu::Type::MAIN, 3) {
    std::vector<std::string> button_texts = { "Play", "Settings", "Exit" };
    for (int i = 0; i < buttonAmount_; ++i) {
        menuItems_[i].setFont(font_);
        menuItems_[i].setString(button_texts[i]);
        menuItems_[i].setFillColor(sf::Color::Black);
        menuItems_[i].setCharacterSize(80);
        // center the text
        sf::FloatRect textBounds = menuItems_[i].getGlobalBounds();
        menuItems_[i].setOrigin(textBounds.width / 2, textBounds.height / 2);
        menuItems_[i].setPosition(SCREEN_CENTER.x, 300 + i * 100);
        if (i == selectedItem_) {
            menuItems_[i].setOutlineColor(sf::Color::White);
            menuItems_[i].setOutlineThickness(5);
        }
    }
};