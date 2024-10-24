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
        Menu(Type type, int buttonAmount);
        virtual void draw(sf::RenderWindow& window) const;
        const int getSelectedItem() const;
        void setSelectedItem(int nextItem);
        const Type getType() const;
        int getItemAtPosition(sf::Vector2f mousePosition) const;
        virtual bool handleMouseClick(sf::Vector2f mousePosition);
        virtual void handleMouseMove(sf::Vector2f mousePosition);
    private:
        Type type_;
    protected:
        sf::Font font_;
        sf::RectangleShape background_;
        sf::Texture backgroundImage_;
        const int buttonAmount_;
        std::vector<sf::Text> menuItems_;
        int selectedItem_ = 0;
        void updateItem(sf::Color fillColor, float scale);
};

#endif // MENU_HPP