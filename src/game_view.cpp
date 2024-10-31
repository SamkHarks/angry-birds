#include "game_view.hpp"
#include "utils.hpp"

GameView::GameView() : sf::RenderWindow(sf::VideoMode(VIEW_WIDTH, VIEW_HEIGHT), "Angry Birds") {
    gameView_ = this->getDefaultView();
    defaultCenter_ = this->getDefaultView().getCenter();
}

void GameView::setGameView() {
    if (updateView_) {
        this->setView(gameView_);
        updateView_ = false;
    }

}

void GameView::updateCamera(const GameModel& model) {
    const World& world = model.getWorld();
    if (model.getState() == GameModel::State::RUNNING) {
        const Bird* activeBird = world.GetBird();
        if (activeBird && activeBird->isMoving()) {
            sf::Vector2f birdPosition = utils::B2ToSfCoords(activeBird->getBody()->GetPosition());
            gameView_.setCenter(std::min(std::max(birdPosition.x, defaultCenter_.x), VIEW_WIDTH * 1.5f), std::min(birdPosition.y, defaultCenter_.y));
            manualControl_ = false;
            updateView_ = true;
        } else if (!manualControl_) {
            gameView_.setCenter(defaultCenter_);
            manualControl_ = true;
            updateView_ = true;
        }
    }
}

void GameView::updateCamera(const sf::Keyboard::Key& code) {
    if (code == sf::Keyboard::Key::Left) {
        auto centerPosition = gameView_.getCenter();
        gameView_.setCenter(std::max(centerPosition.x - 10, defaultCenter_.x), defaultCenter_.y);
        updateView_ = true;
    } else if (code == sf::Keyboard::Key::Right) {
        auto centerPosition = gameView_.getCenter();
        gameView_.setCenter(std::min(centerPosition.x + 10, VIEW_WIDTH * 1.5f), defaultCenter_.y);
        updateView_ = true;
    }
}

// Update HUD elements to keep them in the same position
void GameView::updateHUD(GameModel& model) {
    if (model.getState() == GameModel::State::RUNNING) {
        World& world = model.getWorld();
        world.getScore().updatePosition(*this);
        world.getCannon()->updateTextPosition(*this);
        world.updateRemainingCountPositions(*this);
    }
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
        case GameModel::State::SETTINGS:
            model.getMenu(Menu::Type::SETTINGS).draw(*this);
            break;
        case GameModel::State::RUNNING:
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
