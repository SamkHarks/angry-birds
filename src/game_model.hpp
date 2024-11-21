#ifndef GAME_MODEL_HPP
#define GAME_MODEL_HPP

#include "main_menu.hpp"
#include "game_over.hpp"
#include "settings.hpp"
#include "game_selector.hpp"
#include "pause.hpp"
#include "world.hpp"
#include <type_traits>
#include "level_editor.hpp"

class GameModel {
    public:
        GameModel();   
        void update();
        enum class State {
            MENU,       // The game is at the main menu.
            GAME_SELECTOR, // The player is selecting a game.
            RUNNING,    // The game is currently being played.
            PAUSED,     // The game is paused.
            LOADING,    // The game is loading resources or a new level.
            SETTINGS,   // The player is adjusting settings.
            GAME_OVER,  // The game has ended.
            LEVEL_EDITOR, // The game is at level editor mode.
            QUIT,       // Quit game
        };
        const State& getState() const;
        void setState(State);
        void setState();
        template <typename T>
        T& getMenu(Menu::Type type) {
            static_assert(std::is_base_of<Menu, T>::value, "T must be derived from Menu");
            return static_cast<T&>(getMenu(type));
        }
        template <typename T>
        const T& getMenu(Menu::Type type) const {
            static_assert(std::is_base_of<Menu, T>::value, "T must be derived from Menu");
            return static_cast<const T&>(getMenu(type));
        }
        const Menu& getMenu(const Menu::Type& type) const;
        Menu& getMenu(const Menu::Type& type);
        void setMenu(Menu::Type newMenuType);
        void handleKeyPress(const sf::Keyboard::Key& code);
        void handleKeyRelease();
        World &getWorld();
        const World &getWorld() const;
        void launchBird();
        void handleTextEntered(const sf::Uint32& unicode);
        void handleMouseMove(const sf::Vector2f& mousePosition);
        void handleResize(const sf::RenderWindow& window);
        void handleMouseLeftClick(const sf::Vector2f& mousePosition, const sf::RenderWindow& window);
        void handleMouseRelease(const sf::Mouse::Button& button, const sf::Vector2f& mousePosition);
        void draw(sf::RenderWindow& window) const;
        void switchMenu(Menu::Type type, State state);
        bool isRunning() const;
        bool isPaused() const;
        bool isPausedAtRunning() const;
        bool isLevelEditor() const;
        bool updateView() const;
        void setUpdateView(bool updateView);
        LevelEditor& getLevelEditor();
    private:
        State state_;
        std::unordered_map<Menu::Type, std::unique_ptr<Menu>> menus_;
        Menu *currentMenu_;
        World world_;
        LevelEditor levelEditor_;
        bool updateView_ = false;
        void handleLevelEnd();
        void handleCollisions();
        void handleObjectState();
        void handleBirdState();
        void handleMainMenuState();
        void handleGameOverState();
        void handleSettingsState();
        void handlePauseState();
        void handleGameSelectorState();
        void handleGameSelectorScreenState();
        void handleUserSelectorScreenState();
        void handleLevelSelectorScreenState();
};

#endif // GAME_MODEL_HPP