#include "game.hpp"
#include "world.hpp"
#include "sfml_debug_draw.hpp"

Game::Game() : model_(), view_(), controller_(model_, view_) {}

void Game::run() {
    model_.getWorld().loadLevel("level1.json");
    // TODO: Add possibility to load debug draw
    while (view_.isOpen()) {
        controller_.handleEvents();
        view_.setGameView();
        model_.update();
        view_.updateCamera(model_);
        view_.updateUIElements(model_);
        view_.render(model_);
    }
}