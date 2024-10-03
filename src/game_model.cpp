#include "game_model.hpp"
#include "main_menu.hpp"
#include "game_over.hpp"
#include "utils.hpp"
#include <algorithm>

GameModel::GameModel() : state_(State::MENU), main_menu_(), world_(), gameOverMenu_() {}

void GameModel::update() {
    // TODO: Update game logic here
    switch (state_) {
        case State::RUNNING:
            world_.step();
            world_.getCannon()->update();
            bool isSettled = world_.getIsSettled();
            // TODO: handle game over logic here
            if (isSettled && (world_.getRemainingPigCount() == 0 || world_.getRemainingBirdCount() == 0)) {
                state_ = State::GAME_OVER;
                world_.updateScore(world_.getRemainingBirdCount() * 1000);
                //std::tuple<int,float> results = world_.getScoreAndStars();
            }
            // Check for collisions
            for (b2Contact *ce = world_.getWorld()->GetContactList(); ce; ce = ce->GetNext()) {
                b2Contact *c = ce;

                Object *objA = reinterpret_cast<Object *>(c->GetFixtureA()->GetUserData().pointer);
                Object *objB = reinterpret_cast<Object *>(c->GetFixtureB()->GetUserData().pointer);
                
                objA->handleCollision(objB->getBody()->GetLinearVelocity().Length());
                objB->handleCollision(objA->getBody()->GetLinearVelocity().Length());
            }
            // Check if any objects are destroyed & update score
            for (auto object : world_.getObjects()) {
                if (object->isDestroyed()) {
                    world_.updateScore(object->getDestructionScore());
                    world_.removeObject(object);
                } else if (object->isOutOfBounds()) {
                    world_.removeObject(object);
                }
            }
            // Check if the bird is destroyed
            Bird* bird = world_.GetBird();
            if (bird != nullptr && (bird->isDestroyed() || bird->shouldDestroy())) {
                world_.removeBird();
            }
            // Update the bird and objects
            bird = world_.GetBird();
            if (bird != nullptr) {
                bird->update();
            }
            for (auto object : world_.getObjects()) {
                object->update();
            }
            // Check if the world has settled
            if (bird == nullptr || bird->isLaunched()) {
                isSettled = true;
                for (auto object : world_.getObjects()) {
                    if (object->isMoving()) {
                        isSettled = false;
                        break;
                    }
                }
                if (isSettled && (bird == nullptr || !bird->isMoving())) {
                    world_.setIsSettled(true);
                }
            }
            break;
    }
}

const GameModel::State& GameModel::getState() const {
    return state_;
}

void GameModel::setState(State state) {
    state_ = state;
}

const Menu& GameModel::getMenu(Menu::Type type) const {
    switch (type) {
        case Menu::Type::MAIN:
            return main_menu_;
        case Menu::Type::PAUSE:
            //TODO: return pause menu here, for now return main_menu
            return main_menu_;
            break;
        case Menu::Type::SETTINGS:
            //TODO: return settings here, for now return main_menu
            return main_menu_;
            break;
        case Menu::Type::GAME_OVER:
            return gameOverMenu_;
            break;
        default:
            throw std::invalid_argument("Invalid menu type");
    }
}

Menu& GameModel::getMenu(Menu::Type type) {
    switch (type) {
        case Menu::Type::MAIN:
            return main_menu_;
        case Menu::Type::PAUSE:
            // TODO: return pause menu here,  for now return main_menu
            return main_menu_;
        case Menu::Type::SETTINGS:
            // TODO: return settings here,  for now return main_menu
            return main_menu_;
        case Menu::Type::GAME_OVER:
            return gameOverMenu_;
        default:
            throw std::invalid_argument("Invalid menu type");
    }
}

void GameModel::setMenuSelection(Menu::Type type, sf::Keyboard::Key key) {
    Menu &menu = getMenu(type);
    int currentItem = menu.getSelectedItem();
    menu.setSelectedItem(key == sf::Keyboard::Key::Up
        ? currentItem - 1
        : currentItem + 1
    );
}

void GameModel::setStateFromMenu(Menu::Type type, int selectedItem) {
    switch (type) {
        case Menu::Type::MAIN:
            if (selectedItem == 0) {
                state_ = State::LOADING;
                world_.clearLevel();
                world_.loadLevel("level1.json");
                state_ = State::RUNNING;
            } else if (selectedItem == 1) {
                state_ = State::SETTINGS;
            } else {
                state_ = State::QUIT;
            }
            break;
       case Menu::Type::PAUSE:
            // TODO: handle pause menu update
            break;
        case Menu::Type::SETTINGS:
            // TODO: handle settings update
            break;
        case Menu::Type::GAME_OVER:
            // TODO: handle game over updates properly
            if (selectedItem == 0) {
                // Restart
                state_ = State::LOADING;
                world_.resetLevel();
                state_ = State::RUNNING;
            } else if (selectedItem == 1) {
                // Next Level
                state_ = State::MENU;
            } else if (selectedItem == 2) {
                // Main Menu
                state_ = State::MENU;
            } else {
                // Exit
                state_ = State::QUIT;
            } 
            break;
        default:
            break;
    }
}

World &GameModel::getWorld() {
    return world_;
}

const World &GameModel::getWorld() const {
    return world_;
}

void GameModel::launchBird() {
    Bird* bird = world_.GetBird();
    world_.getCannon()->launchBird(bird);
}

void GameModel::rotateCannon(sf::Vector2f mousePosition) {
    sf::Vector2f canonCenter = utils::B2ToSfCoords(BIRD_INITIAL_POSITION);
    sf::Vector2f difference = mousePosition - canonCenter;
    float direction = utils::getDirection(difference);
    Cannon* cannon = world_.getCannon();
    cannon->setAngle(direction);
}
