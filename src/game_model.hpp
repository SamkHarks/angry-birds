#ifndef GAME_MODEL_HPP
#define GAME_MODEL_HPP

#include "main_menu.hpp"
#include "world.hpp"

class GameModel {
    public:
        GameModel();   
        void update();

        enum class State {
            MENU,       // The game is at the main menu.
            RUNNING,    // The game is currently being played.
            PAUSED,     // The game is paused.
            LOADING,    // The game is loading resources or a new level.
            SETTINGS,   // The player is adjusting settings.
            GAME_OVER,  // The game has ended.
            WIN,        // The player has won the game.
            LOSE,       // The player has lost the game.
            QUIT,       // Quit game
        };

        const State& getState() const;

        void setState(State);

        const Menu& getMenu(Menu::Type type) const;

        void setMenuSelection(Menu::Type type, sf::Keyboard::Key);

        void setStateFromMenu(Menu::Type type, int selectedItem);

        World &getWorld();

        const World &getWorld() const;

        void launchBird(sf::Vector2f mousePosition);

        void rotateCannon(sf::Vector2f mousePosition);

    private:
        State state_;
        MainMenu main_menu_;
        World world_;
};

#endif // GAME_MODEL_HPP