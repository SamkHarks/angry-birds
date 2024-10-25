#ifndef LEVEL_SELECTOR_HPP
#define LEVEL_SELECTOR_HPP

#include <SFML/Graphics.hpp>

struct Level {
    std::string name;
    std::string filename;
};

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
        Level& getSelectedLevel();
        void setLevelIndex(int index);
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
        std::vector<Level> levels_;
        int levelIndex_ = 0;
};

#endif // LEVEL_SELECTOR_HPP
