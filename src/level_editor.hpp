#ifndef LEVEL_EDITOR_HPP
#define LEVEL_EDITOR_HPP

#include <SFML/Graphics.hpp>
#include "level_loader.hpp"
#include "cannon.hpp"
#include <unordered_set>


/**
 * @brief Initializes a Button struct with the given sprite and text.
 * 
 * @param shape The shape to be used for the button.
 * @param text The text to be displayed on the button.
 * @param count the count of items created with the button.
 */
struct Button {
    sf::RectangleShape shape;
    sf::Text text;
    int count = 0;
};

/**
 * @brief A Icon button represented only by a shape.
 * 
 * This button does not contain text, making it useful for icons or 
 * purely graphical buttons in the editor, e,g. save button.
 */
struct IconButton {
    sf::RectangleShape shape;
};

/**
 * @brief A container that holds groups of different types of buttons.
 * 
 * The `ButtonGroup` can hold both standard `Button` types (with text)
 * and `IconButton` types (without text). This can be used for organizing 
 * various UI elements in the level editor.
 */
struct ButtonGroup {
    std::vector<Button> editorButtons;
    std::vector<IconButton> iconButtons;
};

struct Notifications {
    // public members
    std::vector<sf::Text> messages;
    std::vector<sf::Text> errors;
    std::unordered_set<int> ids;
    sf::Clock clock;
    enum class Type {
        MESSAGE,
        ERROR_MESSAGE,
        EMPTY
    };
    Type currentType = Type::EMPTY;
    // public methods
    bool isDisplayed() const {
        return Type::EMPTY != currentType;
    }
    void addNotification(const std::string& message, Type type) {
        // Clear notifications if the type changes
        if (currentType != type) {
            clearNotifications();
            currentType = type;
        }
        // Generate a hash from the message to use as the ID
        size_t id2 = std::hash<std::string>{}(message);
        // Check if the notification already exists based on its hash
        bool newId = ids.insert(id2).second;
        if (!newId) {
            return; // Prevent duplicate notifications
        }
        bool isError = Type::ERROR_MESSAGE == type;
        std::vector<sf::Text>& container = isError ? errors : messages;
        sf::Text text;
        setupText(text, message);
        background.setFillColor(isError ? sf::Color::Red : LIME_GREEN);
        bool updatePositions = updateBackgroundWidth(text);
        updateBackgroundSize(text, container);
        updateTextPositions(updatePositions, container);
        pushNewText(text, container);
        clock.restart();
    }
    void clearNotifications() {
        if (Type::EMPTY == currentType) {
            return; // No notifications to clear
        }
        ids.clear();
        errors.clear();
        messages.clear();
        width = 100;
        height = 40;
        currentType = Type::EMPTY;
    }
    void draw(sf::RenderWindow& window) const {
        if (Type::EMPTY != currentType) {
            window.draw(background);
            if (Type::ERROR_MESSAGE == currentType) {
                for (const auto& error : errors) {
                    window.draw(error);
                }
            } else {
                for (const auto& message : messages) {
                    window.draw(message);
                }
            }
        }
    }
    void init() {
        background.setOutlineColor(sf::Color::Black);
        background.setOutlineThickness(3);
        background.setPosition(VIEW.getCenter().x, 150);
    }
    
    private:
        sf::RectangleShape background;
        int width = 0;
        int height = 40; // padding
        const int VERITCAL_PADDING = 20; // Vertical padding around the text
        const int HORIZONTAL_PADDING = 40; // Horizontal padding around the text
        const int MARGIN = 10;       // Margin between messages
        const int TEXT_SIZE = 40;    // Text character size
        // helper methods
        void setupText(sf::Text& text, const std::string& message) {
            text.setFont(ResourceManager::getInstance().getFont("/assets/fonts/BerkshireSwash-Regular.ttf"));
            text.setString(message);
            text.setCharacterSize(TEXT_SIZE);
            text.setFillColor(sf::Color::White);
            text.setOutlineColor(sf::Color::Black);
            text.setOutlineThickness(3);
        }
        bool updateBackgroundWidth(const sf::Text& text) {
            auto bounds = text.getGlobalBounds();
            if (bounds.width + HORIZONTAL_PADDING > width) {
                width = bounds.width + HORIZONTAL_PADDING;
                return true;
            }
            return false;
        }
        void updateBackgroundSize(const sf::Text& text, const std::vector<sf::Text>& container) {
            auto bounds = text.getGlobalBounds();
            height += container.size() > 0 ? bounds.height + MARGIN : bounds.height;
            background.setSize(sf::Vector2f(width, height));
            background.setOrigin(width/2.f, 0);
        }
        void updateTextPositions(bool updatePositions, std::vector<sf::Text>& container) {
            if (updatePositions) {
                for (auto& item : container) {
                    item.setPosition((background.getPosition().x - width/2) + (HORIZONTAL_PADDING / 2), item.getPosition().y);
                }
            }
        }
        void pushNewText(sf::Text& text, std::vector<sf::Text>& container) {
            auto localBounds = text.getLocalBounds();
    
            // Calculate Y based on the previous containers's Y position or the top of the background
            auto y = container.empty() 
                    ? background.getPosition().y + VERITCAL_PADDING - localBounds.top
                    : container.back().getPosition().y + TEXT_SIZE + MARGIN;

            auto x = background.getPosition().x - width/2 + (HORIZONTAL_PADDING / 2);
            text.setPosition(x, y);
            container.push_back(text);
        }
};

class LevelEditor {
    public:
        enum class Item {
            // Buttons
            RED_BIRD,
            BLUE_BIRD,
            GREEN_BIRD,
            PIG,
            WALL,
            SAVE,
            // Objects
            OBJECT,
            UNDEFINED
        };
        LevelEditor();
        void draw(sf::RenderWindow& window) const;
        int getItemAtPosition(const sf::Vector2f& mousePosition) const;
        bool handleMouseClick(const sf::Vector2f& mousePosition, const sf::RenderWindow& window);
        void handleKeyPress(const sf::Keyboard::Key& key);
        void handleKeyRelease();
        void handleMouseMove(const sf::Vector2f& mousePosition);
        void handleMouseRelease();
        const int getSelectedItem() const;
        void createObject();
        Item convertIndexToItem() const;
        bool isDragging() const;
        void saveLevel(const sf::RenderWindow& window);
        void captureLevelImage(const sf::RenderWindow& window);
        void update();
    private:
        Notifications notifications_;
        LevelCreator levelCreator_;
        LevelObject ground_;
        CannonSprites cannon_;
        std::vector<LevelObject> objects_;
        std::vector<Bird::Type> birdList_;
        ButtonGroup buttonGroups_;
        int selectedItem_ = 0;
        bool isDragging_ = false;
        bool isPressed_ = false;
        sf::Vector2f dragOffset_;
        void updateItem(bool isSelected);
        void updateIntersectingColors();
        ObjectData createObjectData(Object::Type type) const;
        ShapeData createShapeData(Object::Type type) const;
        bool createLevelObject(const ObjectData&, const ShapeData&, LevelObject&);
        bool createSprite(const ObjectData& data, sf::Sprite&) const;
        void updateButtons(bool isAdded);
        const int getObjectIndex() const;
        const int getIconButtonIndex() const;
        void updateObject();
        void checkPosition(LevelObject&);
        b2Vec2 getDimensions(const LevelObject&) const;
        void handleWallButtonPress(const sf::Keyboard::Key& key);
        void removeBird(const Bird::Type& type);
        void removeObject();
        Bird::Type getBirdType(const Item& item) const;
        int NEXT_ID = 0; // Next object ID
};

#endif // LEVEL_EDITOR_HPP