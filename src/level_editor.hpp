#ifndef LEVEL_EDITOR_HPP
#define LEVEL_EDITOR_HPP

#include <SFML/Graphics.hpp>
#include "level_loader.hpp"
#include "cannon.hpp"
#include <unordered_set>

// Constants
const int EDITOR_BUTTONS = 5;
const int ICON_BUTTONS = 2;
const int BUTTONS = EDITOR_BUTTONS + ICON_BUTTONS;
const int SHAPE_SIZE = 53;
const float PIG_RADIUS = 0.3;
const sf::Vector2f PIG_INITIAL_POSITION(210,320);
const sf::Vector2f WALL_INITIAL_POSITION(380, 460);


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
    void init() {
        ResourceManager& resourceManager = ResourceManager::getInstance();
        // Helper lambda for file paths
        auto getFilePath = [&](int i) {
            switch (i) {
                case 0:
                    return "/assets/images/red_bird.png";
                case 1:
                    return "/assets/images/blue_bird.png";
                case 2:
                    return "/assets/images/green_bird.png";
                case 3:
                    return "/assets/images/pig.png";
                case 4:
                    return "/assets/images/box.png";
                case 5:
                    return "/assets/images/save_button1.png";
                case 6:
                    return "/assets/images/settings_button.png";
                default:
                    return "";
            }
        };
        int offset = VIEW.getCenter().x - 200;
        sf::Font &font = resourceManager.getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
        int characterSize = 40; int shapeY = 15; int shapeX = 20; int y = 60;
        for (int i = 0; i < BUTTONS; ++i) {
            sf::RectangleShape shape;
            shape.setTexture(&resourceManager.getTexture(getFilePath(i)));
            // Create icon buttons
            if (i >= EDITOR_BUTTONS) {;
                shape.setSize(sf::Vector2f(SHAPE_SIZE*1.5, SHAPE_SIZE * 1.5));
                shape.setPosition(shapeX, shapeY);
                IconButton iconButton = {shape};
                iconButtons.push_back(iconButton);
                shapeX += 90;
                continue;
            }
            // Create editor buttons
            std::string buttonText = "0";
            int x = offset + (i < 3 ? 20 : 15);
            shape.setSize(sf::Vector2f(SHAPE_SIZE, SHAPE_SIZE));
            shape.setPosition(offset, shapeY);
            sf::Text text;
            text.setFont(font);
            text.setCharacterSize(characterSize);
            text.setFillColor(sf::Color::White);
            text.setOutlineColor(sf::Color::Black);
            text.setOutlineThickness(2);
            text.setString(buttonText);
            text.setPosition(x,y);
            offset += 80;
            Button button = {shape,text};
            editorButtons.push_back(button);
        }
    }

    void handleResize() {
        int offset = VIEW.getCenter().x - 200;
        int shapeY = 15; int shapeX = 20; int y = 60;
        for (int i = 0; i < BUTTONS; ++i) {
            sf::RectangleShape& shape = i >= EDITOR_BUTTONS ? iconButtons[i - EDITOR_BUTTONS].shape : editorButtons[i].shape;
            // Create icon buttons
            if (i >= EDITOR_BUTTONS) {;
                shape.setPosition(shapeX, shapeY);
                shapeX += 90;
                continue;
            }
            // Create editor buttons
            int x = offset + (i < 3 ? 20 : 15);
            shape.setPosition(offset, shapeY);
            sf::Text& text = editorButtons[i].text;
            text.setPosition(x,y);
            offset += 80;
        }
    }

    void updatePosition(const sf::RenderWindow& window) {
        int offset = VIEW.getCenter().x - 200;
        int y = 60; int shapeY = 15; int shapeX = 20;
        for (int i = 0; i < BUTTONS; ++i) {
            sf::RectangleShape& shape = i >= EDITOR_BUTTONS ? iconButtons[i - EDITOR_BUTTONS].shape : editorButtons[i].shape;
             if (i >= EDITOR_BUTTONS) {;
                shape.setPosition(window.mapPixelToCoords(sf::Vector2i(shapeX, shapeY)));
                shapeX += 90;
                continue;
            }
            int x = offset + (i < 3 ? 20 : 15);
            shape.setPosition(window.mapPixelToCoords(sf::Vector2i(offset, shapeY)));
            sf::Text& text = editorButtons[i].text;
            text.setPosition(window.mapPixelToCoords(sf::Vector2i(x, y)));
            offset += 80;
        }
    }
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
        background.setPosition(VIEW.getCenter().x, Y_POSITION);
    }
    void handleResize() {
        background.setPosition(VIEW.getCenter().x, Y_POSITION);
        updateTextPositions(true, messages);
        updateTextPositions(true, errors);
    }
    void updatePosition(const sf::RenderWindow& window) {
        background.setPosition(window.mapPixelToCoords(sf::Vector2i(VIEW.getCenter().x, Y_POSITION)));
        updateTextPositions(errors, window);
        updateTextPositions(messages, window);
    }
    
    private:
        sf::RectangleShape background;
        int width = 0;
        int height = 40; // padding
        const int VERITCAL_PADDING = 20; // Vertical padding around the text
        const int HORIZONTAL_PADDING = 40; // Horizontal padding around the text
        const int MARGIN = 10;       // Margin between messages
        const int TEXT_SIZE = 40;    // Text character size
        const int Y_POSITION = 150; // background position
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
                int x = (background.getPosition().x - width/2) + (HORIZONTAL_PADDING / 2);
                for (auto& item : container) {
                    item.setPosition(x, item.getPosition().y);
                }
            }
        }
        void updateTextPositions(std::vector<sf::Text>& container, const sf::RenderWindow& window) {
            int size = container.size();
            int x = (VIEW.getCenter().x - width / 2) + (HORIZONTAL_PADDING / 2);
            int y = 0;
            for (int i = 0; i < size; ++i) {
                sf::Text& item = container[i];
                auto top = item.getLocalBounds().top;
                if (i == 0) {
                    y = Y_POSITION + VERITCAL_PADDING - top;
                } else {
                    y += TEXT_SIZE + MARGIN;
                }
                item.setPosition(window.mapPixelToCoords(sf::Vector2i(x, y)));
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

struct DragOffsets {
    sf::Vector2f objectDragOffset;
    sf::Vector2f deleteDragOffset;
};

struct Checkbox {
    sf::RectangleShape shape;
    sf::RectangleShape checkmark;
    sf::Text text;
    bool checked = false;
};

struct CheckboxGroup {
    
    enum class Type {
        SHOW_DELETE_BUTTONS
    };
    
    sf::RectangleShape background;
    void init(const IconButton& button) {
        iconButton = button;
        int basePositionY = backgroundPositionY + padding;
        int gap = 50;
        int size = 30;
        ResourceManager& resourceManager = ResourceManager::getInstance();
        sf::Font& font = resourceManager.getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
        sf::Vector2f center = VIEW.getCenter();
        std::vector<std::string> texts = { "Show delete buttons" }; // TODO: Add more checkboxes
        sf::RectangleShape checkmark;
        checkmark.setSize(sf::Vector2f(size, size));
        checkmark.setTexture(&resourceManager.getTexture("/assets/images/checkmark.png"));
        // Get the maximum width of the text & set Text
        std::vector<sf::Text> sfTexts;
        for (int i = 0; i < texts.size(); ++i) {
            sf::Text text;
            text.setFont(font);
            text.setCharacterSize(size);
            text.setFillColor(sf::Color::White);
            text.setOutlineColor(sf::Color::Black);
            text.setOutlineThickness(2);
            text.setString(texts[i]);
            text.setPosition(padding * 2, basePositionY + i * gap);
            sfTexts.push_back(text);
            auto bounds = text.getGlobalBounds();
            shapeX = bounds.left + bounds.width + padding > shapeX ? bounds.left + bounds.width + padding : shapeX;
        }
        // Create checkboxes
        int width = 0;
        int height = 2 * padding;
        for (int i = 0; i < texts.size(); ++i) {
            sf::Text& text = sfTexts[i];
            auto bounds = text.getGlobalBounds();
            // Create the checkbox shape
            sf::RectangleShape shape;
            shape.setSize(sf::Vector2f(size, size));
            shape.setPosition(shapeX, text.getPosition().y);
            checkmark = checkmark;
            checkmark.setPosition(shape.getPosition());
            Checkbox checkbox = { shape, checkmark, text };
            checkBoxes[static_cast<Type>(i)] = checkbox;
            // Update the width of the background
            int newWidth = bounds.width + checkbox.shape.getSize().x + 3 * padding;
            if (newWidth > width) {
                width = newWidth;
            }
            // Update the height of the background
            height += i > 0 ? (gap - size) + size : size;
        }

        background.setSize(sf::Vector2f(width, height));
        background.setFillColor(sf::Color(0, 0, 0, 200));
        background.setPosition(padding, backgroundPositionY);
    }
    void handleResize() {
        background.setPosition(padding, backgroundPositionY);
        for (int i = 0; i < checkBoxes.size(); ++i) {
            Checkbox& checkbox = checkBoxes[static_cast<Type>(i)];
            sf::Text& text = checkbox.text;
            sf::RectangleShape &checkmark = checkbox.checkmark;
            sf::RectangleShape &shape = checkbox.shape;
            shape.setPosition(shapeX, text.getPosition().y);
            checkmark.setPosition(shape.getPosition());
        }
    }
    void updatePosition(const sf::RenderWindow& window) {
        int offset = VIEW.getCenter().x - 200;
        int basePositionY = backgroundPositionY + padding;
        int gap = 50;
        for (int i = 0; i < checkBoxes.size(); ++i) {
            Checkbox& checkbox = checkBoxes[static_cast<Type>(i)];
            sf::Text& text = checkbox.text;
            sf::RectangleShape &checkmark = checkbox.checkmark;
            sf::RectangleShape &shape = checkbox.shape;
            int y = basePositionY + i * gap;
            auto position = window.mapPixelToCoords(sf::Vector2i(shapeX, y));
            shape.setPosition(position);
            checkmark.setPosition(position);
            text.setPosition(window.mapPixelToCoords(sf::Vector2i(padding * 2, y)));
        }
        iconButton.shape.setPosition(window.mapPixelToCoords(sf::Vector2i(110, 15)));
        background.setPosition(window.mapPixelToCoords(sf::Vector2i(padding, backgroundPositionY)));
        
    }
    bool isHovered(const sf::Vector2f& mousePosition) const {
        return isOpen && background.getGlobalBounds().contains(mousePosition);
    }
    void draw(sf::RenderWindow& window) const {
        if (isOpen) {
            window.draw(background);
            for (const auto& checkboxPair : checkBoxes) {
                const auto& checkbox = checkboxPair.second;
                window.draw(checkbox.shape);
                window.draw(checkbox.text);
                if (checkbox.checked) {
                    window.draw(checkbox.checkmark);
                }
            }
        }
    }
    void handleMouseClick(const sf::Vector2f& mousePosition) {
        if (iconButton.shape.getGlobalBounds().contains(mousePosition)) {
            isOpen = !isOpen; // Toggle the settings open/close
            return;
        }
        int index = getItemAtPosition(mousePosition);
        if (index == -1) {
            return; // No item clicked
        }
        // Set the checkbox to checked/unchecked
        Type type = static_cast<Type>(index);
        checkBoxes[type].checked = !checkBoxes[type].checked;
    }
    void handleMouseMove(const sf::Vector2f& mousePosition) {
        int index = getItemAtPosition(mousePosition);
        if (index == -1) {
            updateHoveredItem(false);
            hoveredIndex = -1;
            return;
        }
        updateHoveredItem(false);
        hoveredIndex = index;
        updateHoveredItem(true);
    }
    bool isChecked(Type type) const {
        return checkBoxes.at(type).checked;
    }
    bool getIsOpen() const {
        return isOpen;
    }
    void update(bool isSelected) {
        if (isOpen && !isSelected) {
            updateHoveredItem(false); // ensure no item has hover effect when settings is not selected
        }
    }
    private:
        int backgroundPositionY = 100;
        int padding = 20;
        int shapeX = 0;
        bool isOpen = false;
        IconButton iconButton;
        std::unordered_map<Type, Checkbox> checkBoxes;
        int hoveredIndex = -1;
        void updateHoveredItem(bool isHovered) {
            if (hoveredIndex == -1) return;
            Checkbox& checkbox = checkBoxes[static_cast<Type>(hoveredIndex)];
            if (isHovered) {
                checkbox.shape.setScale(1.05f, 1.05f);
                checkbox.checkmark.setScale(1.05f, 1.05f);
                checkbox.text.setFillColor(LIME_GREEN);
            } else {
                checkbox.shape.setScale(1.f, 1.f);
                checkbox.checkmark.setScale(1.f, 1.f);
                checkbox.text.setFillColor(sf::Color::White);
            }
        }
        // get checkbox index
        int getItemAtPosition(const sf::Vector2f& mousePosition) const {
            for (int i = 0; i < checkBoxes.size(); i++) {
                const Type type = static_cast<Type>(i); 
                if (checkBoxes.at(type).shape.getGlobalBounds().contains(mousePosition)
                || checkBoxes.at(type).text.getGlobalBounds().contains(mousePosition)
                || checkBoxes.at(type).checkmark.getGlobalBounds().contains(mousePosition)) {
                    return i;
                }
            }
            return -1;
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
            SETTINGS,
            // Objects
            OBJECT,
            DELETE_OBJECT,
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
        void handleResize();
        const int getSelectedItem() const;
        void createObject();
        Item convertIndexToItem() const;
        bool isDragging() const;
        void saveLevel(const sf::RenderWindow& window);
        void captureLevelImage(const sf::RenderWindow& window);
        void update();
        void updateHUD(const sf::RenderWindow& window);
    private:
        CheckboxGroup settings_;
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
        DragOffsets dragOffsets_;
        void updateItem(bool isSelected);
        void updateIntersectingColors();
        ObjectData createObjectData(Object::Type type) const;
        ShapeData createShapeData(Object::Type type) const;
        bool createLevelObject(const ObjectData&, const ShapeData&, LevelObject&);
        bool createSprite(const ObjectData& data, sf::Sprite&) const;
        void updateButtons(bool isAdded);
        const int getObjectIndex(const Item& item) const;
        const int getIconButtonIndex() const;
        void updateObject();
        void checkPosition(LevelObject&);
        b2Vec2 getDimensions(const LevelObject&) const;
        void handleWallButtonPress(const sf::Keyboard::Key& key);
        void removeBird(const Bird::Type& type);
        void removeObject();
        Bird::Type getBirdType(const Item& item) const;
        int NEXT_ID = 0; // Next object ID
        sf::Sprite addDeleteButton(const sf::Sprite& sprite, const Object::Type& type);
        void setDeleteButtonPosition(const sf::Sprite& newSprite, LevelObject& object);
};

#endif // LEVEL_EDITOR_HPP