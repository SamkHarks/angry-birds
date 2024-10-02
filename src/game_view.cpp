#include "game_view.hpp"
#include "utils.hpp"

GameView::GameView() : sf::RenderWindow(sf::VideoMode(VIEW_WIDTH, VIEW_HEIGHT), "Angry Birds") {
    gameView_ = this->getDefaultView();
    defaultCenter_ = this->getDefaultView().getCenter();
}

void GameView::setGameView() {
    this->setView(gameView_);
}

void GameView::updateCamera(const GameModel& model) {
    const World& world = model.getWorld();
    const Bird* activeBird = world.GetBird();
    if (activeBird && activeBird->isMoving()) {
        sf::Vector2f birdPosition = utils::B2ToSfCoords(activeBird->getBody()->GetPosition());
        gameView_.setCenter(std::min(std::max(birdPosition.x, defaultCenter_.x), VIEW_WIDTH * 1.f), std::min(birdPosition.y, defaultCenter_.y));
    } else {
        gameView_.setCenter(SCREEN_CENTER);
    }
}

// Update UI elements here that are not part of the game world (and require window coordinates)
void GameView::updateUIElements(GameModel& model) {
    // Update the position of the score
    World& world = model.getWorld();
    world.getScore().updatePosition(*this);
    // TODO: Update other UI elements
}


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
