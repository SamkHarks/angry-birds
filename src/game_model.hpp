#ifndef GAME_MODEL_HPP
#define GAME_MODEL_HPP

class GameModel {
    public:
        GameModel();   
        void update();

        enum class State {
            MENU,
            RUNNING,
            PAUSED,
        }; 

    private:
        State state_;
};

#endif // GAME_MODEL_HPP