#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "menu.hpp"

class MainMenu: public Menu {
    public:
        MainMenu();
        void draw(sf::RenderWindow& window) const override;
    private:
        sf::RectangleShape woodenSign_;
        sf::Texture signImage_;
};

#endif // MAIN_MENU_HPP