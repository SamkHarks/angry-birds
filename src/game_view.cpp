#include "game_view.hpp"

GameView::GameView() : window_(sf::VideoMode(1024, 720), "Angry Birds") {}


void GameView::render() {
    window_.clear(sf::Color::Blue);
    draw();
    window_.display();
}

void GameView::draw() {
    // TODO: Draw game objects here
}

sf::RenderWindow& GameView::getWindow() {
    return window_;
}