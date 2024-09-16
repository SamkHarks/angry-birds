#include "game.hpp"
#include "world.hpp"
#include "sfml_debug_draw.hpp"

Game::Game() : model_(), view_(), controller_(model_, view_) {}

void Game::run() {
    model_.getWorld().loadLevel("level1.txt");
    // TODO: Add possibility to load debug draw
    while (view_.isOpen()) {
        controller_.handleEvents();
        model_.update();
        view_.render(model_);
    }
}