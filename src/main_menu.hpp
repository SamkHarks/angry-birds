#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "menu.hpp"
#include "level_selector.hpp"
#include "user_selector.hpp"

class MainMenu: public Menu {
    public:
        MainMenu();
        void draw(sf::RenderWindow& window) const override;
        LevelSelector& getLevelSelector();
        UserSelector& getUserSelector();
        void handleResize() override;
    private:
        virtual const std::vector<std::string>& getButtonNames() const override;
};

#endif // MAIN_MENU_HPP