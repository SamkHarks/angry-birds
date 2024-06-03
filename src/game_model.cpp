#include "game_model.hpp"
#include "main_menu.hpp"

GameModel::GameModel() : state_(State::MENU), main_menu_(0), world_() {}

void GameModel::update() {
    // TODO: Update game logic here
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