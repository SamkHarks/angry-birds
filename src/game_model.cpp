#include "game_model.hpp"
#include "utils.hpp"
#include <algorithm>

GameModel::GameModel() :
    state_(State::MENU),
    mainMenu_(),
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

const Menu& GameModel::getMenu(const Menu::Type& type) const {
    switch (type) {
        case Menu::Type::MAIN:
            return mainMenu_;
        case Menu::Type::PAUSE:
            //TODO: return pause menu here, for now return main_menu
            return mainMenu_;
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

Menu& GameModel::getMenu(const Menu::Type& type) {
    switch (type) {
        case Menu::Type::MAIN:
            return mainMenu_;
        case Menu::Type::PAUSE:
            // TODO: return pause menu here,  for now return main_menu
            return mainMenu_;
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
            gameSelector_.handleKeyPress(code);
            break;
        case State::GAME_OVER:
            gameOverMenu_.handleKeyPress(code);
            break;
        case State::MENU:
            mainMenu_.handleKeyPress(code);
            break;
        case State::SETTINGS:
        case State::PAUSED:
            break;
        default:
            break;
    }
}

// handle menu actions based on the current state and selected item
void GameModel::setState() {
    switch (state_) {
        case State::MENU: 
            handleMainMenuState();
            break;
        case State::GAME_OVER:
            handleGameOverState();
            break;
        case State::GAME_SELECTOR:
            handleGameSelectorState();
            break;
        case State::SETTINGS:
            handleSettingsState();
            break;
        case State::PAUSED:
            handlePauseState();
            break;
        default:
            break;
    }
}

void GameModel::handleMainMenuState() {
    auto selectedItem = mainMenu_.getSelectedItem();
    if (selectedItem == 0) {
        gameSelector_.updateMenuItems();
        gameSelector_.setScreen(GameSelector::Screen::GAME_SELECTOR);
        state_ = State::GAME_SELECTOR;
    } else if (selectedItem == 1) {
        state_ = State::SETTINGS;
    } else {
        state_ = State::QUIT;
    }
}

void GameModel::handleGameOverState() {
    const int selectedItem = gameOverMenu_.getSelectedItem();
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
        mainMenu_.updateMusic(sf::SoundSource::Status::Playing);
    } else if (selectedItem == exitIndex) {
        // Exit
        state_ = State::QUIT;
    } 
}

void GameModel::handleGameSelectorScreenState() {
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
}

void GameModel::handleUserSelectorScreenState() {
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
}

void GameModel::handleLevelSelectorScreenState() {
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
            mainMenu_.updateMusic(sf::SoundSource::Status::Stopped);
            state_ = State::RUNNING;
            break;
        case LevelSelector::Item::NEXT: 
            levelSelector.setLevel(LevelSelector::Item::NEXT);
            break;
        case LevelSelector::Item::PREV:
            levelSelector.setLevel(LevelSelector::Item::PREV);
            break;
    }
}

void GameModel::handleGameSelectorState() {
    switch (gameSelector_.getScreen()) {
        case GameSelector::Screen::GAME_SELECTOR:
            handleGameSelectorScreenState();
            break;
        case GameSelector::Screen::USER_SELECTOR:
            handleUserSelectorScreenState();
            break;
        case GameSelector::Screen::LEVEL_SELECTOR:
            handleLevelSelectorScreenState();
            break;
    }
}

// TODO: Add implementation later
void GameModel::handleSettingsState() {
    if (settings_.getSelectedItem() == 0) {
        state_ = State::MENU;
    }
}

// TODO: Add implementation later
void GameModel::handlePauseState() {

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

void GameModel::handleTextEntered(const sf::Uint32& unicode) {
    if (state_ == State::GAME_SELECTOR) {
        gameSelector_.handleTextEntered(unicode);
    }
}

void GameModel::handleMouseMove(const sf::Vector2f& mousePosition) {
    switch(state_) {
        case State::RUNNING:
            world_.handleMouseMove(mousePosition);
            break;
        case GameModel::State::MENU:
            mainMenu_.handleMouseMove(mousePosition);
            break;
        case GameModel::State::GAME_OVER:
            gameOverMenu_.handleMouseMove(mousePosition);
            break;
        case GameModel::State::GAME_SELECTOR:
            gameSelector_.handleMouseMove(mousePosition);
            break;
        default:
            break;
    }
}

void GameModel::handleResize() {
    world_.handleResize();
    mainMenu_.handleResize();
    gameOverMenu_.handleResize();
    gameSelector_.handleResize();
    settings_.handleResize();
}

void GameModel::handleMouseLeftClick(const sf::Vector2f& mousePosition) {
     switch(state_) {
        case State::RUNNING:
            world_.getCannon()->startLaunch();
            break;
        case State::MENU:
            if (mainMenu_.handleMouseClick(mousePosition)) {
                setState();
            }
            break;
        case State::GAME_SELECTOR:
            if (gameSelector_.handleMouseClick(mousePosition)) {
                setState();
            }
            break;
        case GameModel::State::GAME_OVER:
            if (gameOverMenu_.handleMouseClick(mousePosition)) {
                setState();
            }
            break;
        case State::SETTINGS:
            if (settings_.handleMouseClick(mousePosition)) {
                setState();
            }
            break;
        case State::PAUSED:
            // TODO: Implement mouse actions in Pause menu
            break;
        default:
            break;
    }
}