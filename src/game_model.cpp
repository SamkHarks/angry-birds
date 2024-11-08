#include "game_model.hpp"
#include "utils.hpp"
#include <algorithm>

GameModel::GameModel() :
    state_(State::MENU),
    world_() {
        menus_[Menu::Type::MAIN] = std::make_unique<MainMenu>();
        menus_[Menu::Type::GAME_SELECTOR] = std::make_unique<GameSelector>();
        menus_[Menu::Type::SETTINGS] = std::make_unique<Settings>();
        menus_[Menu::Type::GAME_OVER] = std::make_unique<GameOver>();
        currentMenu_ = menus_[Menu::Type::MAIN].get();  // Start with main menu
    }

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
    setMenu(Menu::Type::GAME_OVER);
    auto &gameOverMenu = static_cast<GameOver&>(getMenu(Menu::Type::GAME_OVER));
    auto &gameSelector = static_cast<GameSelector&>(getMenu(Menu::Type::GAME_SELECTOR));
    world_.updateScore(world_.getRemainingBirdCount() * 1000);
    world_.getScore().setStars(world_.getStars());
    world_.getScore().setLevelEndText(world_.getLevelName());
    world_.saveHighScore(world_.getScore().getCurrentScore());
    if (world_.updatePlayer()) {
        gameSelector.getUserSelector().savePlayer();
    }
    gameOverMenu.setScoreManager(&world_.getScore());
    gameOverMenu.setHasNextlevel(gameSelector.getLevelSelector().hasNextLevel());
    gameOverMenu.updateMenuItems();
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
    return *menus_.at(type);
}

Menu& GameModel::getMenu(const Menu::Type& type) {
    return *menus_.at(type);
}

void GameModel::setMenu(Menu::Type newMenuType) {
    currentMenu_ = menus_.at(newMenuType).get();
}

void GameModel::handleKeyPress(const sf::Keyboard::Key& code) {
    currentMenu_->handleKeyPress(code);  
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
   auto selectedItem = currentMenu_->getSelectedItem();
    if (selectedItem == 0) {
        auto& gameSelector = static_cast<GameSelector&>(getMenu(Menu::Type::GAME_SELECTOR));
        gameSelector.updateMenuItems();
        gameSelector.setScreen(GameSelector::Screen::GAME_SELECTOR);
        // Change the state and active menu
        state_ = State::GAME_SELECTOR;
        setMenu(Menu::Type::GAME_SELECTOR);
    } else if (selectedItem == 1) {
        state_ = State::SETTINGS;
        setMenu(Menu::Type::SETTINGS);
    } else {
        state_ = State::QUIT;
    }
}

void GameModel::handleGameOverState() {
    auto &gameOverMenu = static_cast<GameOver&>(getMenu(Menu::Type::GAME_OVER));
    const int selectedItem = gameOverMenu.getSelectedItem();
    // TODO: handle game over updates properly
    int nextLevelIndex = 1;
    int mainMenuIndex = 2;
    int exitIndex = 3;
    if (!gameOverMenu.hasNextLevel()) {
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
        auto &gameSelector = static_cast<GameSelector&>(getMenu(Menu::Type::GAME_SELECTOR));
        // Next Level
        world_.clearLevel();
        world_.loadLevel(gameSelector.getLevelSelector().getNextLevel().filename);
        state_ = State::RUNNING;
    } else if (selectedItem == mainMenuIndex) {
        // Main Menu
        state_ = State::MENU;
        setMenu(Menu::Type::MAIN);
        currentMenu_->updateMusic(sf::SoundSource::Status::Playing);
    } else if (selectedItem == exitIndex) {
        // Exit
        state_ = State::QUIT;
    } 
}

void GameModel::handleGameSelectorScreenState() {
    auto &gameSelector = static_cast<GameSelector&>(getMenu(Menu::Type::GAME_SELECTOR));
    auto item = gameSelector.getSelectedItem();
    switch (item) {
        case GameSelector::Item::NEW_GAME: {
            UserSelector& userSelector = gameSelector.getUserSelector();
            gameSelector.setScreen(GameSelector::Screen::USER_SELECTOR);
            userSelector.setScreen(UserSelector::Screen::NEW_PLAYER);
            userSelector.initializeScreen();
            break;
        }
        case GameSelector::Item::LOAD_GAME: {
            UserSelector& userSelector = gameSelector.getUserSelector();
            gameSelector.setScreen(GameSelector::Screen::USER_SELECTOR);
            userSelector.setScreen(UserSelector::Screen::LOAD_PLAYER);
            userSelector.initializeScreen();
            break;
        }
        case GameSelector::Item::CONTINUE:
            gameSelector.getLevelSelector().updateLevel();
            gameSelector.setScreen(GameSelector::Screen::LEVEL_SELECTOR);
            break;
        case GameSelector::Item::BACK:
            state_ = State::MENU;
            setMenu(Menu::Type::MAIN);
            break;
    }
}

void GameModel::handleUserSelectorScreenState() {
    auto &gameSelector = static_cast<GameSelector&>(getMenu(Menu::Type::GAME_SELECTOR));
    UserSelector& userSelector = gameSelector.getUserSelector();
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
                gameSelector.initializeLevelSelector();
            }
            break;
        case UserSelector::Screen::LOAD_PLAYER:
            if (item == UserSelector::Item::BACK) {
                gameSelector.setScreen(GameSelector::Screen::GAME_SELECTOR);
            } else if (item == UserSelector::Item::PLAYER_NAME) {
                userSelector.loadPlayer();
                gameSelector.initializeLevelSelector();
            } else if (item == UserSelector::Item::PREV) {
                userSelector.setIndexRange(UserSelector::Item::PREV);
            } else if (item == UserSelector::Item::NEXT) {
                userSelector.setIndexRange(UserSelector::Item::NEXT);
            }
            break;
    }
}

void GameModel::handleLevelSelectorScreenState() {
    auto & gameSelector = static_cast<GameSelector&>(getMenu(Menu::Type::GAME_SELECTOR));
    LevelSelector& levelSelector = gameSelector.getLevelSelector();
    LevelSelector::Item item = levelSelector.getSelectedItem();
    switch (item) {
        case LevelSelector::Item::BACK:
            gameSelector.updateMenuItems();
            gameSelector.setScreen(GameSelector::Screen::GAME_SELECTOR);
            break;
        case LevelSelector::Item::LEVEL:
            world_.clearLevel();
            world_.loadLevel(levelSelector.getSelectedLevel().filename);
            world_.setPlayer(gameSelector.getUserSelector().getPlayer());
            getMenu(Menu::Type::MAIN).updateMusic(sf::SoundSource::Status::Stopped);
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
    const auto& gameSelector = static_cast<GameSelector&>(getMenu(Menu::Type::GAME_SELECTOR));
    switch (gameSelector.getScreen()) {
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
    if (currentMenu_->getSelectedItem() == 0) {
        setMenu(Menu::Type::MAIN);
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
        currentMenu_->handleTextEntered(unicode);
    }
}

void GameModel::handleMouseMove(const sf::Vector2f& mousePosition) {
   if (state_ == State::RUNNING) {
        world_.handleMouseMove(mousePosition);
    } else {
        currentMenu_->handleMouseMove(mousePosition);
    }
}

void GameModel::handleResize() {
    world_.handleResize();
    for (auto& menu : menus_) {
        menu.second->handleResize();
    }
}

void GameModel::handleMouseLeftClick(const sf::Vector2f& mousePosition) {
   if (state_ == State::RUNNING) {
        world_.getCannon()->startLaunch();
    } else {
        if (currentMenu_->handleMouseClick(mousePosition)) {
            setState();
        }
    }
}

void GameModel::draw(sf::RenderWindow& window) const {
    if (state_ == State::RUNNING) {
        world_.draw(window);
    } else {
        currentMenu_->draw(window);
    }
}
