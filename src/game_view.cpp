#include "game_view.hpp"

GameView::GameView() : sf::RenderWindow(sf::VideoMode(1024, 720), "Angry Birds") {}


void GameView::render(const GameModel& model) {
    this->clear(sf::Color::Blue);
    draw(model);
    this->display();
}

void GameView::draw(const GameModel& model) {
    // TODO: Draw game objects here
    switch (model.getState()) {
        case GameModel::State::MENU:
            model.getMenu(Menu::Type::MAIN).draw(*this);
            break;
        case GameModel::State::RUNNING:
            // TODO: Draw game objects
            break;
        case GameModel::State::PAUSED:
            // TODO: Draw pause menu
            break;
    }
}
