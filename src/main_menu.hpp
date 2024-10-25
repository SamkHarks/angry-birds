#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "menu.hpp"
#include "level_selector.hpp"
#include "user_selector.hpp"

class MainMenu: public Menu {
    public:
        enum class Screen {
            MAIN,
            USER_SELECTOR,
            LEVEL_SELECTOR
        };
        MainMenu();
        void draw(sf::RenderWindow& window) const override;
        LevelSelector& getLevelSelector();
        UserSelector& getUserSelector();
        void setScreen(Screen screen);
        Screen getScreen() const;
        void handleMouseMove(sf::Vector2f mousePosition) override;
        bool handleMouseClick(sf::Vector2f mousePosition) override;
    private:
        sf::RectangleShape woodenSign_;
        sf::Texture signImage_;
        std::vector<sf::Text> title_;
        LevelSelector levelSelector_;
        UserSelector userSelector_;
        Screen screen_ = Screen::MAIN;
};

#endif // MAIN_MENU_HPP