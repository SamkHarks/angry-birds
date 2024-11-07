#include "game_model.hpp"
#include "utils.hpp"
#include <algorithm>

GameModel::GameModel() :
    state_(State::MENU),
    main_menu_(),
    world_(),
    gameOverMenu_(),
    settings_(),
    gameSelector_() {}

void GameModel::update() {
    switch (state_) {
        case State::RUNNING:
            world_.step();

            // Update the cannon if the bird is not launched
            Bird* bird = world_.GetBird();
            if (bird != nullptr && !bird->isLaunched()) {
                world_.getCannon()->update();
            }

            // Check if level is ended and handle level ending
            if (world_.isSettled()) {
                handleLevelEnd();
            }
            // Handle collisions
            handleCollisions();

            // Handle object state and bird state
            handleObjectState();
            handleBirdState();

            break;
    }
}

void GameModel::handleLevelEnd() {
    // Set state, update score and player, and set Score for level end menu
    state_ = State::GAME_OVER;
    world_.updateScore(world_.getRemainingBirdCount() * 1000);
    world_.getScore().setStars(world_.getStars());
    world_.getScore().setLevelEndText(world_.getLevelName());
    world_.saveHighScore(world_.getScore().getCurrentScore());
    if (world_.updatePlayer()) {
        gameSelector_.getUserSelector().savePlayer();
    }
    gameOverMenu_.setScoreManager(&world_.getScore());
    gameOverMenu_.setHasNextlevel(gameSelector_.getLevelSelector().hasNextLevel());
    gameOverMenu_.updateMenuItems();
}

void GameModel::handleCollisions() {
    // Check for collisions
    for (b2Contact *ce = world_.getWorld()->GetContactList(); ce; ce = ce->GetNext()) {
        b2Contact *c = ce;

        Object *objA = reinterpret_cast<Object *>(c->GetFixtureA()->GetUserData().pointer);
        Object *objB = reinterpret_cast<Object *>(c->GetFixtureB()->GetUserData().pointer);
        
        objA->handleCollision(objB->getBody()->GetLinearVelocity().Length());
        objB->handleCollision(objA->getBody()->GetLinearVelocity().Length());
    }
}

void GameModel::handleObjectState() {
    // Check if any objects are destroyed or out of bounds otherwise update them
    for (auto object : world_.getObjects()) {
        if (object->shouldRemove()) {
            if (object->getType() == Object::Type::Pig) {
                world_.updateRemainingCounts(object->getTypeAsChar());
            }
            world_.updateScore(object->getDestructionScore());
            world_.removeObject(object);
        } else {
            object->update();
        }
    }
}

void GameModel::handleBirdState() {
    // Check if the bird is destroyed or out of bounds otherwise update it
    Bird *bird = world_.GetBird();
    if (bird != nullptr) {
        if (bird->shouldRemove()) {
            world_.updateRemainingCounts(bird->getTypeAsChar());
            world_.removeBird();
        } else {
            bird->update();
        }
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
        case Menu::Type::SETTINGS:
            return settings_;
        case Menu::Type::GAME_OVER:
            return gameOverMenu_;
        case Menu::Type::GAME_SELECTOR:
            return gameSelector_;
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
            return settings_;
        case Menu::Type::GAME_OVER:
            return gameOverMenu_;
        case Menu::Type::GAME_SELECTOR:
            return gameSelector_;
        default:
            throw std::invalid_argument("Invalid menu type");
    }
}

void GameModel::handleKeyPress(const sf::Keyboard::Key& code) {
    switch (state_) {
        case State::GAME_SELECTOR:
            switch (gameSelector_.getScreen()) {
                case GameSelector::Screen::GAME_SELECTOR:
                    setMenuSelection(Menu::Type::GAME_SELECTOR, code);
                    break;
                case GameSelector::Screen::LEVEL_SELECTOR: {
                    LevelSelector& levelSelector = gameSelector_.getLevelSelector();
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
                case GameSelector::Screen::USER_SELECTOR: {
                    UserSelector& userSelector = gameSelector_.getUserSelector();
                    const UserSelector::Item& item = userSelector.convertIndexToItem();
                    if (userSelector.getScreen() == UserSelector::Screen::NEW_PLAYER) {
                        if (code == sf::Keyboard::Key::Left) {
                            userSelector.setSelectedItem(item == UserSelector::Item::ACCEPT ? 1 : 0);
                        } else if (code == sf::Keyboard::Key::Right) {
                            userSelector.setSelectedItem(item == UserSelector::Item::CANCEL ? 0 : 1);
                        }
                    } else {
                        const UserSelector::IndexRange& range = userSelector.getIndexRange();
                        int playerCountRange = PLAYER_INDEX_START + range.end;
                        int selectedItem = userSelector.getSelectedItem();
                        if (code == sf::Keyboard::Key::Up) {
                            int nextItem;
                            if (item == UserSelector::Item::PREV || item == UserSelector::Item::NEXT) {
                                nextItem = PLAYER_INDEX_START + range.start;
                            } else if (item == UserSelector::Item::BACK) {
                                nextItem = playerCountRange - 1;
                            } else if (selectedItem - 1 < PLAYER_INDEX_START + range.start) {
                                nextItem = static_cast<int>(UserSelector::Item::BACK);
                            } else {
                                nextItem = selectedItem - 1;
                            }
                            userSelector.setSelectedItem(nextItem);
                        } else if (code == sf::Keyboard::Key::Down) {
                            int nextItem;
                            if (item == UserSelector::Item::BACK || item == UserSelector::Item::PREV || item == UserSelector::Item::NEXT) {
                                nextItem = PLAYER_INDEX_START + range.start;
                            } else if (selectedItem + 1 >= playerCountRange) {
                                nextItem = static_cast<int>(UserSelector::Item::BACK);
                            } else {
                                nextItem = selectedItem + 1;
                            }
                            userSelector.setSelectedItem(nextItem);
                        }
                    }
                    break;
                }

            }
            break;
        case State::GAME_OVER:
            setMenuSelection(Menu::Type::GAME_OVER, code);
            break;
        case State::MENU:
            setMenuSelection(Menu::Type::MAIN, code);
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

// TODO: Implement the setState function and refactor the code to smaller functions
void GameModel::setState() {
    switch (state_) {
        case State::MENU: {
           auto selectedItem = main_menu_.getSelectedItem();
            if (selectedItem == 0) {
                gameSelector_.updateMenuItems();
                gameSelector_.setScreen(GameSelector::Screen::GAME_SELECTOR);
                state_ = State::GAME_SELECTOR;
            } else if (selectedItem == 1) {
                state_ = State::SETTINGS;
            } else {
                state_ = State::QUIT;
            }
            break;
        }
        case State::GAME_OVER: {
            auto selectedItem = gameOverMenu_.getSelectedItem();
            // TODO: handle game over updates properly
            int nextLevelIndex = 1;
            int mainMenuIndex = 2;
            int exitIndex = 3;
            if (!gameOverMenu_.hasNextLevel()) {
                nextLevelIndex = -100; // Set to invalid index
                mainMenuIndex = 1;
                exitIndex = 2;
            }
            if (selectedItem == 0) {
                // Restart
                world_.clearLevel();
                world_.resetLevel();
                state_ = State::RUNNING;
            } else if (selectedItem == nextLevelIndex) {
                // Next Level
                world_.clearLevel();
                world_.loadLevel(gameSelector_.getLevelSelector().getNextLevel().filename);
                state_ = State::RUNNING;
            } else if (selectedItem == mainMenuIndex) {
                // Main Menu
                state_ = State::MENU;
                main_menu_.updateMusic(sf::SoundSource::Status::Playing);
            } else if (selectedItem == exitIndex) {
                // Exit
                state_ = State::QUIT;
            } 
            break;
        }
        case State::GAME_SELECTOR: {
            switch (gameSelector_.getScreen()) {
                case GameSelector::Screen::GAME_SELECTOR: {
                    auto item = gameSelector_.getSelectedItem();
                    switch (item) {
                        case GameSelector::Item::NEW_GAME: {
                            UserSelector& userSelector = gameSelector_.getUserSelector();
                            gameSelector_.setScreen(GameSelector::Screen::USER_SELECTOR);
                            userSelector.setScreen(UserSelector::Screen::NEW_PLAYER);
                            userSelector.initializeScreen();
                            break;
                        }
                        case GameSelector::Item::LOAD_GAME: {
                            UserSelector& userSelector = gameSelector_.getUserSelector();
                            gameSelector_.setScreen(GameSelector::Screen::USER_SELECTOR);
                            userSelector.setScreen(UserSelector::Screen::LOAD_PLAYER);
                            userSelector.initializeScreen();
                            break;
                        }
                        case GameSelector::Item::CONTINUE:
                            gameSelector_.getLevelSelector().updateLevel();
                            gameSelector_.setScreen(GameSelector::Screen::LEVEL_SELECTOR);
                            break;
                        case GameSelector::Item::BACK:
                            state_ = State::MENU;
                        break;
                    }
                    break;
                }
                case GameSelector::Screen::USER_SELECTOR: {
                    UserSelector& userSelector = gameSelector_.getUserSelector();
                    const UserSelector::Item& item = userSelector.convertIndexToItem();
                    switch (userSelector.getScreen()) {
                        case UserSelector::Screen::NEW_PLAYER:
                            // Check if player is set and accepted
                            if (userSelector.isPlayerSet()) {
                                if (item == UserSelector::Item::ACCEPT) {
                                    userSelector.setPlayerAccepted(true);
                                } else if (item == UserSelector::Item::CANCEL) {
                                    userSelector.clearPlayer();
                                }
                            // set player if player is not set
                            } else {
                                userSelector.setPlayer();
                            }
                            // if player is accepted, save new player and move to level selector
                            if (userSelector.isPlayerAccepted()) {
                                if (userSelector.isNewPlayer()) {
                                    userSelector.savePlayer();
                                }
                                gameSelector_.initializeLevelSelector();
                            }
                            break;
                        case UserSelector::Screen::LOAD_PLAYER:
                            if (item == UserSelector::Item::BACK) {
                                gameSelector_.setScreen(GameSelector::Screen::GAME_SELECTOR);
                            } else if (item == UserSelector::Item::PLAYER_NAME) {
                                userSelector.loadPlayer();
                                gameSelector_.initializeLevelSelector();
                            } else if (item == UserSelector::Item::PREV) {
                                userSelector.setIndexRange(UserSelector::Item::PREV);
                            } else if (item == UserSelector::Item::NEXT) {
                                userSelector.setIndexRange(UserSelector::Item::NEXT);
                            }
                            break;
                    }
                    
                    break;
                }
                case GameSelector::Screen::LEVEL_SELECTOR: {
                    LevelSelector& levelSelector = gameSelector_.getLevelSelector();
                    LevelSelector::Item item = levelSelector.getSelectedItem();
                    switch (item) {
                        case LevelSelector::Item::BACK:
                            gameSelector_.updateMenuItems();
                            gameSelector_.setScreen(GameSelector::Screen::GAME_SELECTOR);
                            break;
                        case LevelSelector::Item::LEVEL:
                            world_.clearLevel();
                            world_.loadLevel(levelSelector.getSelectedLevel().filename);
                            world_.setPlayer(gameSelector_.getUserSelector().getPlayer());
                            main_menu_.updateMusic(sf::SoundSource::Status::Stopped);
                            state_ = State::RUNNING;
                            break;
                        case LevelSelector::Item::NEXT: 
                            levelSelector.setLevel(LevelSelector::Item::NEXT);
                            break;
                        case LevelSelector::Item::PREV:
                            levelSelector.setLevel(LevelSelector::Item::PREV);
                            break;
                    }
                    break;
                }
            }
        }
        break;
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
    if (state_ == State::GAME_SELECTOR && gameSelector_.getScreen() == GameSelector::Screen::USER_SELECTOR) {
        // Handle backspace
        if (unicode == 8) {
            std::string currentText = gameSelector_.getUserSelector().getPlayerText();
            if (!currentText.empty()) {
                currentText.pop_back();
                gameSelector_.getUserSelector().setPlayerText(currentText);
            }
        // Handle printable characters
        } else if (unicode >= 32 && unicode < 128) {
            std::string currentText = gameSelector_.getUserSelector().getPlayerText();
            // Max length of player name is 12
            if (currentText.size() < 13) {
                currentText += static_cast<char>(unicode);
                gameSelector_.getUserSelector().setPlayerText(currentText);
            }

        }
    }
}
