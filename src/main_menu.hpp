#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "menu.hpp"

class MainMenu: public Menu {
    public:
        MainMenu();
        void draw(sf::RenderWindow& window) const override;

    private:
        const static int button_amount_ = 3;
        sf::Text menu_items_[button_amount_];
};

#endif // MAIN_MENU_HPP