#include "main_menu.hpp"


MainMenu::MainMenu() : Menu(Menu::Type::MAIN) {
    std::string button_texts[button_amount_] = { "Play", "Settings", "Exit" };
    for (int i = 0; i < button_amount_; ++i) {
        menu_items_[i].setFont(font_);
        menu_items_[i].setString(button_texts[i]);
        menu_items_[i].setFillColor(sf::Color::Black);
        menu_items_[i].setCharacterSize(80);
        menu_items_[i].setPosition(400, 200 + i * 100);
    }
};

void MainMenu::draw(sf::RenderWindow &window) const {
    window.draw(background_);
    for (auto menu_item : menu_items_)
    {
        window.draw(menu_item);
    }
};
