#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "menu.hpp"

class MainMenu: public Menu {
    public:
        MainMenu();
        void draw(sf::RenderWindow& window) const override;
        bool setPlayer();
        bool isPlayerSet() const;
        void setPromptPlayer(bool promptPlayer);
        bool isPromptVisible() const;
        std::string getPlayer() const;
        void setPromptText(const std::string& text);
        void setPlayerText(const std::string& text);
        std::string getPlayerText() const;
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
};

#endif // MAIN_MENU_HPP