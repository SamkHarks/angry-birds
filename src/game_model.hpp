#ifndef GAME_MODEL_HPP
#define GAME_MODEL_HPP

#include "main_menu.hpp"

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
        };

        const State& getState() const;

        const Menu& getMenu(Menu::Type type) const;

    private:
        State state_;
        MainMenu main_menu_;
};

#endif // GAME_MODEL_HPP