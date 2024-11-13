#ifndef LEVEL_EDITOR_HPP
#define LEVEL_EDITOR_HPP

#include <SFML/Graphics.hpp>
#include "level_loader.hpp"


/**
 * @brief Level objects used for creating level
 * 
 * @param sprite The sprite representing the visual appearance of the object
 * @param data The data representing the object's properties and used for saving to the level file
 */
struct LevelObject {
    sf::Sprite sprite;
    ObjectData data;
};

/**
 * @brief Initializes a Button struct with the given sprite and text.
 * 
 * @param shape The shape to be used for the button.
 * @param text The text to be displayed on the button.
 */
struct Button {
    sf::RectangleShape shape;
    sf::Text text;
    int count = 0;
};

class LevelEditor {
    public:
        enum class Item {
            RED_BIRD,
            BLUE_BIRD,
            GREEN_BIRD,
            PIG,
            WALL,
            OBJECT,
            UNDEFINED
        };
        LevelEditor();
        void draw(sf::RenderWindow& window) const;
        int getItemAtPosition(const sf::Vector2f& mousePosition) const;
        bool handleMouseClick(const sf::Vector2f& mousePosition);
        void handleMouseMove(const sf::Vector2f& mousePosition);
        void handleMouseRelease();
        const int getSelectedItem() const;
        void createObject();
        Item convertIndexToItem() const;
        bool isDragging() const;
    private:
        LevelObject ground_;
        std::vector<LevelObject> objects_;
        std::vector<Bird::Type> birdList_;
        std::vector<Button> buttons_;
        int selectedItem_ = 0;
        bool isDragging_ = false;
        sf::Vector2f dragOffset_;
        void updateItem(bool isSelected);
        ObjectData createObjectData(Object::Type type) const;
        bool createLevelObject(const ObjectData&, LevelObject&) const;
        bool createSprite(const ObjectData& data, sf::Sprite&) const;
        void updateButtons();
        const int getObjectIndex() const;
        void updateObject();
        void checkPosition(LevelObject&);
};

#endif // LEVEL_EDITOR_HPP