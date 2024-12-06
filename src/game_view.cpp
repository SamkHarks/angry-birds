#include "game_view.hpp"
#include "game_model.hpp"
#include "utils.hpp"

GameView::GameView() : sf::RenderWindow(sf::VideoMode(VIEW.getWidth(), VIEW.getHeight()), "Angry Birds") {
    gameView_ = this->getDefaultView();
    defaultCenter_ = this->getDefaultView().getCenter();
}

void GameView::setDefaultView() {
    gameView_.setCenter(defaultCenter_);
    this->setView(gameView_);
}

void GameView::setGameView() {
    this->setView(gameView_);
    eventDispatcher_->dispatch(Event(Event::Type::UpdateHUD));
}

void GameView::setGameView(const sf::View& view) {
    gameView_ = view;
    this->setView(view);
    auto x = view.getSize().x;
    defaultCenter_ = view.getCenter();
    eventDispatcher_->dispatch(Event(Event::Type::UpdateHUD));
}

void GameView::updateCamera(GameModel& model) {
    const World& world = model.getWorld();
    if (model.isRunning()) {
        const Bird* activeBird = world.GetBird();
        if (activeBird && activeBird->isMoving()) {
            sf::Vector2f birdPosition = utils::B2ToSfCoords(activeBird->getBody()->GetPosition());
            auto height = VIEW.getHeight();
            auto worldTop = -height + 200; // Take into account how worlds bg is positioned
            gameView_.setCenter(std::min(std::max(birdPosition.x, defaultCenter_.x), WORLD_WIDTH - (gameView_.getSize().x*0.5f)), std::max(std::min(birdPosition.y, defaultCenter_.y), worldTop + (0.5f*height)));
            manualControl_ = false;
            eventDispatcher_->dispatch(Event(Event::Type::UpdateView));
        } else if (!manualControl_) {
            gameView_.setCenter(defaultCenter_);
            manualControl_ = true;
            eventDispatcher_->dispatch(Event(Event::Type::UpdateView));
        }
    }
}

void GameView::updateCamera(const sf::Keyboard::Key& code) {
    if (!manualControl_) {
        return; // Don't update camera if it's not in manual control
    }
    if (code == sf::Keyboard::Key::Left) {
        auto centerPosition = gameView_.getCenter();
        gameView_.setCenter(std::max(centerPosition.x - 10, defaultCenter_.x), centerPosition.y);
        eventDispatcher_->dispatch(Event(Event::Type::UpdateView));
    } else if (code == sf::Keyboard::Key::Right) {
        auto centerPosition = gameView_.getCenter();
        gameView_.setCenter(std::min(centerPosition.x + 10, WORLD_WIDTH - (gameView_.getSize().x*0.5f)), centerPosition.y);
        eventDispatcher_->dispatch(Event(Event::Type::UpdateView));
    } else if (code == sf::Keyboard::Key::Up) {
        auto centerPosition = gameView_.getCenter();
        auto height = VIEW.getHeight();
        auto worldTop = -height + 200; // Take into account how worlds bg is positioned
        gameView_.setCenter(centerPosition.x, std::max(centerPosition.y - 10, worldTop + (0.5f*height)));
        eventDispatcher_->dispatch(Event(Event::Type::UpdateView));
    } else if (code == sf::Keyboard::Key::Down) {
        auto centerPosition = gameView_.getCenter();
        gameView_.setCenter(centerPosition.x, std::min(centerPosition.y + 10, defaultCenter_.y));
        eventDispatcher_->dispatch(Event(Event::Type::UpdateView));
    }
}

// Update HUD elements to keep them in the same position
void GameView::updateHUD(GameModel& model) {
    if (model.isRunning() || model.isPausedAtRunning()) {
        World& world = model.getWorld();
        world.updateHUD(*this);
    } else if (model.isLevelEditor() || model.isPaused()) {
        model.getLevelEditor().updateHUD(*this);
    }
}

void GameView::handleResize(const float& width, const float& height) {
    VIEW.setSize(width, height);
    gameView_.setSize(width, height);
    gameView_.setCenter(width / 2, height / 2);
    this->setView(gameView_);
    defaultCenter_ = gameView_.getCenter();
    eventDispatcher_->dispatch(Event(Event::Type::UpdateHUD));
}

void GameView::render(const GameModel& model) {
    this->clear(sf::Color::Blue);
    draw(model);
    this->display();
}

void GameView::draw(const GameModel& model) {
    if (model.getState() == GameModel::State::QUIT) {
        this->close();
    } else {
        model.draw(*this);
    }
}

void GameView::setEventDispatcher(EventDispatcher* eventDispatcher) {
    eventDispatcher_ = eventDispatcher;
}