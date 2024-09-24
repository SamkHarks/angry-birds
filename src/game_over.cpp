#include "game_over.hpp"
#include "utils.hpp"
#include <vector>

GameOver::GameOver() : Menu(Menu::Type::GAME_OVER, 4) {
    std::vector<std::string> button_texts = { "Restart", "Next Level", "Main Menu", "Exit" };
    for (int i = 0; i < buttonAmount_; ++i) {
        int itemWidth = button_texts[i].length() * 80;
        float x = (VIEW_WIDTH - itemWidth) / 2;
        menuItems_[i].setFont(font_);
        menuItems_[i].setString(button_texts[i]);
        menuItems_[i].setFillColor(sf::Color::Black);
        menuItems_[i].setCharacterSize(80);
        menuItems_[i].setPosition(x , 200 + i * 100);
        if (i == selectedItem_) {
            menuItems_[i].setOutlineColor(sf::Color::White);
            menuItems_[i].setOutlineThickness(5);
        }
    }
}