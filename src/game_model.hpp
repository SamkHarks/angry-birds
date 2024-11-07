#ifndef GAME_MODEL_HPP
#define GAME_MODEL_HPP

#include "main_menu.hpp"
#include "game_over.hpp"
#include "settings.hpp"
#include "game_selector.hpp"
#include "world.hpp"

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
            QUIT,       // Quit game
        };

        const State& getState() const;

        void setState(State);

        void setState();

        const Menu& getMenu(Menu::Type type) const;

        Menu& getMenu(Menu::Type type);

        void setMenuSelection(Menu::Type type, sf::Keyboard::Key);

        void handleKeyPress(const sf::Keyboard::Key& code);

        World &getWorld();

        const World &getWorld() const;

        void launchBird();

        void rotateCannon(const sf::Vector2f& mousePosition);

        void handleTextEntered(sf::Uint32 unicode);

    private:
        State state_;
        MainMenu main_menu_;
        GameOver gameOverMenu_;
        GameSelector gameSelector_;
        Settings settings_;
        World world_;
        void handleLevelEnd();
        void handleCollisions();
        void handleObjectState();
        void handleBirdState();
};

#endif // GAME_MODEL_HPP