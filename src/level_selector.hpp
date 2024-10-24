#ifndef LEVEL_SELECTOR_HPP
#define LEVEL_SELECTOR_HPP

#include <SFML/Graphics.hpp>

class LevelSelector {
    public:
        enum class Item {
            LEVEL,
            BACK,
            PREV,
            NEXT,
            UNDEFINED
        };
        LevelSelector();
        void draw(sf::RenderWindow& window) const;
        const Item getSelectedItem() const;
        void setSelectedItem(Item nextItem);
        Item getItemAtPosition(sf::Vector2f mousePosition) const;
        bool handleMouseClick(sf::Vector2f mousePosition);
        void handleMouseMove(sf::Vector2f mousePosition);
    private:
        void updateItem(bool isSelected);
        sf::RectangleShape level_;
        sf::Texture levelImage_;
        std::vector<sf::Text> menuItems_;
        std::vector<sf::RectangleShape> buttons_;
        sf::Texture rightArrow_;
        sf::Texture leftArrow_;
        sf::Font font_;
        Item selectedItem_ = Item::LEVEL;
        std::string currentLevel_ = "Level 1";
};

#endif // LEVEL_SELECTOR_HPP
