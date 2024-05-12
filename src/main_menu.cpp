#include "main_menu.hpp"


MainMenu::MainMenu(int currentItem) : selectedItem_(currentItem), Menu(Menu::Type::MAIN) {
    std::string button_texts[button_amount_] = { "Play", "Settings", "Exit" };
    for (int i = 0; i < button_amount_; ++i) {
        menu_items_[i].setFont(font_);
        menu_items_[i].setString(button_texts[i]);
        menu_items_[i].setFillColor(sf::Color::Black);
        menu_items_[i].setCharacterSize(80);
        menu_items_[i].setPosition(400, 200 + i * 100);
        if (i == selectedItem_) {
            menu_items_[i].setOutlineColor(sf::Color::White);
            menu_items_[i].setOutlineThickness(5);
        }
    }
};

void MainMenu::draw(sf::RenderWindow &window) const {
    window.draw(background_);
    for (auto menu_item : menu_items_)
    {
        window.draw(menu_item);
    }
};

void MainMenu::setSelectedItem(int nextItem) {
    updateItem(sf::Color::Black, 0);
    if (nextItem < 0) {
        selectedItem_ = button_amount_ - 1;
    } else if (nextItem >= button_amount_) {
        selectedItem_ = 0;
    } else {
        selectedItem_ = nextItem;
    }
    updateItem(sf::Color::White, 5);
}

const int MainMenu::getSelectedItem() const {
    return selectedItem_;
}

void MainMenu::updateItem(sf::Color color, int thickness) {
    menu_items_[selectedItem_].setOutlineColor(color);
    menu_items_[selectedItem_].setOutlineThickness(thickness);
}
