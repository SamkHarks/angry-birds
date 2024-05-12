#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "menu.hpp"

class MainMenu: public Menu {
    public:
        MainMenu(int currentItem);
        void draw(sf::RenderWindow& window) const override;
        void setSelectedItem(int);
        const int getSelectedItem() const;
    private:
        const static int button_amount_ = 3;
        sf::Text menu_items_[button_amount_];
        int selectedItem_;
        void updateItem(sf::Color color, int thickness);
};

#endif // MAIN_MENU_HPP