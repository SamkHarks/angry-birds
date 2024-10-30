#include "game_model.hpp"
#include "main_menu.hpp"
#include "game_over.hpp"
#include "utils.hpp"
#include <algorithm>

GameModel::GameModel() : state_(State::MENU), main_menu_(), world_(), gameOverMenu_(), settings_() {}

void GameModel::update() {
    // TODO: Update game logic here
    switch (state_) {
        case State::RUNNING:
            world_.step();
            // Update the cannon if the bird is not launched
            Bird* bird = world_.GetBird();
            if (bird != nullptr && !bird->isLaunched()) {
                world_.getCannon()->update();
            }
            // Check if the level is over, update and set Score for level end menu
            bool isSettled = world_.getIsSettled();
            bool isLevelCleared = world_.getRemainingPigCount() == 0 || world_.getAliveBirdCount() == 0;
            if (isLevelCleared && isSettled) {
                state_ = State::GAME_OVER;
                world_.updateScore(world_.getRemainingBirdCount() * 1000);
                world_.getScore().setStars(world_.getStars());
                world_.getScore().setLevelEndText(world_.getLevelName());
                world_.saveHighScore(world_.getScore().getCurrentScore());
                if (world_.updatePlayer()) {
                    main_menu_.getUserSelector().savePlayer(world_.getPlayer(), true);
                }
                gameOverMenu_.setScoreManager(&world_.getScore());
            }
            // Check for collisions
            for (b2Contact *ce = world_.getWorld()->GetContactList(); ce; ce = ce->GetNext()) {
                b2Contact *c = ce;

                Object *objA = reinterpret_cast<Object *>(c->GetFixtureA()->GetUserData().pointer);
                Object *objB = reinterpret_cast<Object *>(c->GetFixtureB()->GetUserData().pointer);
                
                objA->handleCollision(objB->getBody()->GetLinearVelocity().Length());
                objB->handleCollision(objA->getBody()->GetLinearVelocity().Length());
            }
            // Check if any objects are destroyed & update score or remove them if they are out of bounds
            for (auto object : world_.getObjects()) {
                if (object->isDestroyed()) {
                    if (object->getType() == Object::Type::Pig) {
                        world_.updateRemainingCounts(object->getTypeAsChar());
                    }
                    world_.updateScore(object->getDestructionScore());
                    world_.removeObject(object);
                } else if (object->isOutOfBounds()) {
                    if (object->getType() == Object::Type::Pig) {
                        world_.updateRemainingCounts(object->getTypeAsChar());
                    }
                    world_.updateScore(object->getDestructionScore());
                    world_.removeObject(object);
                }
            }
            // Check if the bird is destroyed
            bird = world_.GetBird();
            if (bird != nullptr && (bird->isDestroyed() || bird->shouldDestroy())) {
                world_.updateRemainingCounts(bird->getTypeAsChar());
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
            if (bird == nullptr || bird->isLaunched() || world_.getRemainingPigCount() == 0) {
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
            return settings_;
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
            return settings_;
        case Menu::Type::GAME_OVER:
            return gameOverMenu_;
        default:
            throw std::invalid_argument("Invalid menu type");
    }
}

void GameModel::handleKeyPress(const sf::Keyboard::Key& code) {
    switch (state_) {
        case State::MENU:
            switch (main_menu_.getScreen()) {
                case MainMenu::Screen::MAIN:
                    setMenuSelection(Menu::Type::MAIN, code);
                    break;
                case MainMenu::Screen::LEVEL_SELECTOR: {
                    LevelSelector& levelSelector = main_menu_.getLevelSelector();
                    if (code == sf::Keyboard::Key::Left) {
                        levelSelector.setSelectedItem(levelSelector.getSelectedItem() == LevelSelector::Item::PREV
                            ? LevelSelector::Item::NEXT
                            : LevelSelector::Item::PREV
                        );
                    } else if (code == sf::Keyboard::Key::Right) {
                        levelSelector.setSelectedItem(levelSelector.getSelectedItem() == LevelSelector::Item::NEXT
                            ? LevelSelector::Item::PREV
                            : LevelSelector::Item::NEXT
                        );
                    } else if (code == sf::Keyboard::Key::Up) {
                        levelSelector.setSelectedItem(levelSelector.getSelectedItem() == LevelSelector::Item::LEVEL
                            ? LevelSelector::Item::BACK
                            : LevelSelector::Item::LEVEL
                        );
                    } else if (code == sf::Keyboard::Key::Down) {
                        levelSelector.setSelectedItem(levelSelector.getSelectedItem() == LevelSelector::Item::BACK
                            ? LevelSelector::Item::LEVEL
                            : LevelSelector::Item::BACK
                        );
                    }
                    break;
                }
                case MainMenu::Screen::USER_SELECTOR:
                    if (code == sf::Keyboard::Key::Left || code == sf::Keyboard::Key::Right) {
                        main_menu_.getUserSelector().setSelectedItem(main_menu_.getUserSelector().getSelectedItem() == 0 ? 1 : 0);
                    }
                    break;
            }
            break;
        case State::GAME_OVER:
            setMenuSelection(Menu::Type::GAME_OVER, code);
            break;
        case State::SETTINGS:
        case State::PAUSED:
            break;
        default:
            break;
    }
}

void GameModel::setMenuSelection(Menu::Type type, sf::Keyboard::Key key) {
    Menu &menu = getMenu(type);
    int currentItem = menu.getSelectedItem();
    if (key == sf::Keyboard::Key::Up) {
        menu.setSelectedItem(currentItem - 1);
    } else if (key == sf::Keyboard::Key::Down) {
        menu.setSelectedItem(currentItem + 1);
    }
}

void GameModel::setState() {
    switch (state_) {
        case State::MENU: {
            // Handle main menus different screens
            switch (main_menu_.getScreen()) {
                // Handle the main menu screen
                case MainMenu::Screen::MAIN: {
                    auto selectedItem = main_menu_.getSelectedItem();
                     if (selectedItem == 0) {
                        main_menu_.setScreen(MainMenu::Screen::LEVEL_SELECTOR);
                    } else if (selectedItem == 1) {
                        state_ = State::SETTINGS;
                    } else {
                        state_ = State::QUIT;
                    }
                    break;
                }
                // Handle the user selector screen
                case MainMenu::Screen::USER_SELECTOR: {
                    UserSelector& userSelector = main_menu_.getUserSelector();
                    // Check if the player is set or not
                    if (userSelector.isPlayerSet()) {
                        if (userSelector.getSelectedItem() == 0) {
                            userSelector.setPlayerAccepted(true);
                        } else {
                            userSelector.clearPlayer();
                        }
                    // If the player is not set, set the player
                    } else {
                        userSelector.setPlayer();
                    }
                    // If the player is accepted, load the level
                    if (userSelector.isPlayerAccepted()) {
                        world_.clearLevel();
                        world_.loadLevel(main_menu_.getLevelSelector().getSelectedLevel().filename);
                        world_.setPlayer(userSelector.getPlayer());
                        if (userSelector.isNewPlayer()) {
                            userSelector.savePlayer(userSelector.getPlayer());
                        }
                        main_menu_.updateMusic(sf::SoundSource::Status::Stopped);
                        state_ = State::RUNNING;
                    }
                    break;
                }
                // Handle the level selector screen
                case MainMenu::Screen::LEVEL_SELECTOR: {
                    LevelSelector& levelSelector = main_menu_.getLevelSelector();
                    LevelSelector::Item item = levelSelector.getSelectedItem();
                    switch (item) {
                        case LevelSelector::Item::BACK:
                            main_menu_.setScreen(MainMenu::Screen::MAIN);
                            break;
                        case LevelSelector::Item::LEVEL:
                            main_menu_.setScreen(MainMenu::Screen::USER_SELECTOR);
                            break;
                        case LevelSelector::Item::NEXT: 
                            levelSelector.setLevel(LevelSelector::Item::NEXT);
                            break;
                        case LevelSelector::Item::PREV:
                            levelSelector.setLevel(LevelSelector::Item::PREV);
                            break;
                    }

                }
            }
            break;
        }
        case State::GAME_OVER: {
            auto selectedItem = gameOverMenu_.getSelectedItem();
            // TODO: handle game over updates properly
            if (selectedItem == 0) {
                // Restart
                world_.resetLevel();
                state_ = State::RUNNING;
            } else if (selectedItem == 1) {
                // Next Level
                state_ = State::MENU;
                UserSelector& userSelector = main_menu_.getUserSelector();
                userSelector.setPlayerAccepted(false);
                userSelector.setPromptText("Player: " + userSelector.getPlayer().name + " already exists. Load player?");
                main_menu_.setScreen(MainMenu::Screen::MAIN);
                main_menu_.updateMusic(sf::SoundSource::Status::Playing); 
            } else if (selectedItem == 2) {
                // Main Menu
                state_ = State::MENU;
                UserSelector& userSelector = main_menu_.getUserSelector();
                userSelector.setPlayerAccepted(false);
                userSelector.setPromptText("Player: " + userSelector.getPlayer().name + " already exists. Load player?");
                main_menu_.setScreen(MainMenu::Screen::MAIN);
                main_menu_.updateMusic(sf::SoundSource::Status::Playing);
            } else {
                // Exit
                state_ = State::QUIT;
            } 
            break;
        }
        // TODO: handle other states
        case State::SETTINGS:
            if (settings_.getSelectedItem() == 0) {
                state_ = State::MENU;
            }
        case State::PAUSED:
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

void GameModel::handleTextEntered(sf::Uint32 unicode) {
    if (state_ == State::MENU && main_menu_.getScreen() == MainMenu::Screen::USER_SELECTOR) {
        // Handle backspace
        if (unicode == 8) {
            std::string currentText = main_menu_.getUserSelector().getPlayerText();
            if (!currentText.empty()) {
                currentText.pop_back();
                main_menu_.getUserSelector().setPlayerText(currentText);
            }
        // Handle printable characters
        } else if (unicode >= 32 && unicode < 128) {
            std::string currentText = main_menu_.getUserSelector().getPlayerText();
            // Max length of player name is 12
            if (currentText.size() < 13) {
                currentText += static_cast<char>(unicode);
                main_menu_.getUserSelector().setPlayerText(currentText);
            }

        }
    }
}
