#include "game_view.hpp"
#include "utils.hpp"

GameView::GameView() : sf::RenderWindow(sf::VideoMode(VIEW_WIDTH, VIEW_HEIGHT), "Angry Birds") {}


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
            model.getWorld().draw(*this);
            break;
        case GameModel::State::PAUSED:
            // TODO: Draw pause menu
            break;
        case GameModel::State::GAME_OVER:
            model.getMenu(Menu::Type::GAME_OVER).draw(*this);
            break;
        case GameModel::State::QUIT:
            this->close();
            break;
    }
}
