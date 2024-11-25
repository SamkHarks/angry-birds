#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

struct MenuItemLayout {
    int yOffset; // The y offset of the menu item from reference point.
    int characterSize; // The character size of the menu item.
    float gap; // The gap between menu items.
};

class Menu {
    public:
        enum class Type {
            MAIN,           // The main menu.
            GAME_SELECTOR,  // The game selector menu.
            PAUSE,          // The pause menu.
            SETTINGS,       // The settings menu.
            GAME_OVER,      // The game over menu.
        };
        Menu(Type type);
        virtual void draw(sf::RenderWindow& window) const = 0;
        const int getSelectedItem() const;
        void setSelectedItem(int nextItem);
        const Type getType() const;
        int getItemAtPosition(const sf::Vector2f& mousePosition) const;
        virtual bool handleMouseClick(const sf::Vector2f& mousePosition);
        virtual void handleMouseMove(const sf::Vector2f& mousePosition);
        virtual void handleKeyPress(const sf::Keyboard::Key& code);
        void updateMusic(sf::SoundSource::Status status);
        virtual void handleResize();
        virtual void handleResize(const sf::RenderWindow&) {};
        virtual void handleTextEntered(const sf::Uint32& unicode) {}; // Handle text input, default is empty
        void updateMenuItems();
    private:
        Type type_;
    protected:
        sf::RectangleShape woodenSign_;
        sf::RectangleShape background_;
        sf::Texture backgroundImage_;
        sf::SoundBuffer backgroundMusicBuffer_;
        sf::Sound backgroundMusic_;
        int buttonAmount_ = 0;
        std::vector<sf::Text> menuItems_;
        std::vector<sf::Text> title_;
        int selectedItem_ = 0;
        void updateItem(sf::Color fillColor, float scale);
        void setTitle(const std::string& title, float radius, int y);
        virtual const std::vector<std::string>& getButtonNames() const = 0;
        virtual const MenuItemLayout& getMenuItemLayout() const;
};

#endif // MENU_HPP