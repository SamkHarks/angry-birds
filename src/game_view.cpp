#include "game_view.hpp"

GameView::GameView() : sf::RenderWindow(sf::VideoMode(1024, 720), "Angry Birds") {}


void GameView::render(const GameModel::State& state) {
    this->clear(sf::Color::Blue);
    draw(state);
    this->display();
}

void GameView::draw(const GameModel::State& state) {
    // TODO: Draw game objects here
    switch (state) {
        case GameModel::State::MENU:
            // TODO: Draw menu
            break;
        case GameModel::State::RUNNING:
            // TODO: Draw game objects
            break;
        case GameModel::State::PAUSED:
            // TODO: Draw pause menu
            break;
    }
}
