#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "menu.hpp"

class Settings : public Menu {
    public:
        Settings();
        void draw(sf::RenderWindow& window) const override;
    private:
        // Add private members
};

#endif // SETTINGS_HPP