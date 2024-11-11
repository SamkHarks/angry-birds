#ifndef LEVEL_SELECTOR_HPP
#define LEVEL_SELECTOR_HPP

#include <SFML/Graphics.hpp>
#include "user_selector.hpp"

struct Level {
    std::string name;
    std::string filename;
    std::string image;
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
        Item getItemAtPosition(const sf::Vector2f& mousePosition) const;
        bool handleMouseClick(const sf::Vector2f& mousePosition);
        void handleMouseMove(const sf::Vector2f& mousePosition);
        Level& getSelectedLevel();
        void setLevel(Item item);
        void handleResize();
        bool hasNextLevel() const;
        Level& getNextLevel();
        void updateLevel();
        void setPlayer(const std::shared_ptr<Player>& player);
        void handleKeyPress(const sf::Keyboard::Key& code);
    private:
        void updateItem(bool isSelected);
        std::weak_ptr<Player> player_; // User selector is the owner
        std::vector<sf::Sprite> stars_;
        std::vector<sf::Text> signText_;
        std::vector<sf::RectangleShape> sign_;
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
        int starIndex_ = 0;
        void setLevelText();
        void setLevelImage();
        void setLevelIndex(int index);
        void setLevelStarIndex();
        void setLevelHighScore();
        void setSignTexts();
        void drawStars(sf::RenderWindow& window) const;
        bool isButtonDisabled(int index) const;
};

#endif // LEVEL_SELECTOR_HPP
