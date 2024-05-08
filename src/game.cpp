#include "game.hpp"

Game::Game() : _window(sf::VideoMode(1024, 720), "Angry Birds") {
    _window.setFramerateLimit(144);
}

void Game::run() {
    while (_window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents() {
    for (auto event = sf::Event{}; _window.pollEvent(event);) {
        if (event.type == sf::Event::Closed) {
            _window.close();
        }
    }
}

void Game::update() {
    // TODO: Update game logic here
}

void Game::render() {
    _window.clear(sf::Color::Blue);
    _window.display();
}