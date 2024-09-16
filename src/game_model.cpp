#include "game_model.hpp"
#include "main_menu.hpp"
#include "utils.hpp"

GameModel::GameModel() : state_(State::MENU), main_menu_(0), world_() {}

void GameModel::update() {
    // TODO: Update game logic here
    switch (state_) {
        case State::RUNNING:
            world_.step();
            // TODO: Update game objects
            b2Body* bird_body = world_.GetBird()->getBody();
            b2Vec2 bird_position = world_.GetBird()->getBody()->GetPosition();
            sf::Sprite& sprite = world_.GetBird()->getSprite();
            sf::Vector2f bird_position_pixels = utils::B2ToSfCoords(bird_position);
            float radians = bird_body->GetAngle();
            float deg = utils::RadiansToDegrees(radians);
            sprite.setRotation(deg);
            sprite.setPosition(bird_position_pixels.x, bird_position_pixels.y);
            for (auto object : world_.getObjects()) {
                if (object->getType() != Object::Type::Ground) {
                    b2Body* body = object->getBody();
                    b2Vec2 position = body->GetPosition();
                    sf::Sprite& sprite = object->getSprite();
                    sf::Vector2f position_pixels = utils::B2ToSfCoords(position);
                    sprite.setPosition(position_pixels.x, position_pixels.y);
                    float radians = body->GetAngle();
                    float deg = utils::RadiansToDegrees(radians);
                    sprite.setRotation(deg);
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
            //TODO: return pause menu here
            break;
        case Menu::Type::SETTINGS:
            //TODO: return settings here
            break;
        case Menu::Type::GAME_OVER:
            //TODO: return game over here
            break;
        default:
            throw std::invalid_argument("Invalid menu type");
    }
}

void GameModel::setMenuSelection(Menu::Type type, sf::Keyboard::Key key) {
    switch (type) {
        case Menu::Type::MAIN: {
            int currentItem = main_menu_.getSelectedItem();
            main_menu_.setSelectedItem(key == sf::Keyboard::Key::Up
                ? currentItem - 1
                : currentItem + 1
            );
        }
        case Menu::Type::PAUSE:
            // TODO: handle pause menu update
            break;
        case Menu::Type::SETTINGS:
            // TODO: handle settings update
            break;
        case Menu::Type::GAME_OVER:
            // TODO: handle game over update
            break;
        default:
            break;
    }
}

void GameModel::setStateFromMenu(Menu::Type type, int selectedItem) {
    switch (type) {
        case Menu::Type::MAIN:
            if (selectedItem == 0) {
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
            // TODO: handle game over update
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