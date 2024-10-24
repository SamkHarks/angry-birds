#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "menu.hpp"
#include "level_selector.hpp"

class MainMenu: public Menu {
    public:
        enum class Screen {
            MAIN,
            USER_SELECTOR,
            LEVEL_SELECTOR
        };
        MainMenu();
        void draw(sf::RenderWindow& window) const override;
        bool setPlayer();
        bool isPlayerSet() const;
        std::string getPlayer() const;
        void setPromptText(const std::string& text);
        void setPlayerText(const std::string& text);
        std::string getPlayerText() const;
        void handleMenuSelection(sf::Keyboard::Key key);
        LevelSelector& getLevelSelector();
        void setScreen(Screen screen);
        Screen getScreen() const;
        void handleMouseMove(sf::Vector2f mousePosition) override;
        bool handleMouseClick(sf::Vector2f mousePosition) override;
    private:
        sf::RectangleShape woodenSign_;
        sf::Texture signImage_;
        std::vector<sf::Text> title_;
        std::string player_;
        sf::Text promptText_;
        sf::Text playerText_;
        sf::RectangleShape caret_;
        mutable sf::Clock caretClock_;
        mutable bool caretVisible_ = false;
        bool promptPlayer_ = false;
        LevelSelector levelSelector_;
        Screen screen_ = Screen::MAIN;
};

#endif // MAIN_MENU_HPP