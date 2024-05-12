#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>

class Menu {
    public:
        enum class Type {
            MAIN,       // The main menu.
            PAUSE,      // The pause menu.
            SETTINGS,   // The settings menu.
            GAME_OVER,  // The game over menu.
        };
        Menu(Type type);
        virtual void draw(sf::RenderWindow& window) const = 0;
        virtual const int getSelectedItem() const = 0;
        const Type getType() const;
    private:
        Type type_;
    protected:
        sf::Font font_;
        sf::RectangleShape background_;
        sf::Texture background_image_;
};

#endif // MENU_HPP