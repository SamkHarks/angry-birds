#include "game_model.hpp"
#include "utils.hpp"
#include "game_view.hpp"
#include <algorithm>

GameModel::GameModel() :
    state_(State::MENU),
    world_(),
    levelEditor_() {
        menus_[Menu::Type::MAIN] = std::make_unique<MainMenu>();
        menus_[Menu::Type::GAME_SELECTOR] = std::make_unique<GameSelector>();
        menus_[Menu::Type::SETTINGS] = std::make_unique<Settings>();
        menus_[Menu::Type::GAME_OVER] = std::make_unique<GameOver>();
        menus_[Menu::Type::PAUSE] = std::make_unique<Pause>();
        currentMenu_ = menus_[Menu::Type::MAIN].get();  // Start with main menu
    }

void GameModel::update() {
    if (isRunning()) {
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
        world_.handleCollisions();

        // Handle object state and bird state
        world_.handleObjectState();
        world_.handleBirdState();
    } else if (isLevelEditor()) {
        levelEditor_.update();
    }
}

void GameModel::handleLevelEnd() {
    // Set state, update score and player, and set Score for level end menu
    updateView_ = true; // Force view update to center the view
    switchMenu(Menu::Type::GAME_OVER, State::GAME_OVER);
    auto &gameOverMenu = getMenu<GameOver>(Menu::Type::GAME_OVER);
    auto &gameSelector = getMenu<GameSelector>(Menu::Type::GAME_SELECTOR);
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

void GameModel::switchMenu(Menu::Type type, State state) {
    state_ = state;
    setMenu(type);
}

void GameModel::handleKeyPress(const sf::Keyboard::Key& code) {
    switch (code) {
        case sf::Keyboard::Key::Up:
        case sf::Keyboard::Key::Down:
        case sf::Keyboard::Key::Left:
        case sf::Keyboard::Key::Right:
            if (!isRunning()) {
                currentMenu_->handleKeyPress(code);
            }
            break;
        case sf::Keyboard::Key::Enter:
                setState();
            break;
        case sf::Keyboard::Key::P:
        case sf::Keyboard::Key::Escape:
            if (isRunning()) {
                switchMenu(Menu::Type::PAUSE, State::PAUSED);
                world_.handleKeyPress(code);
            } else if (isPausedAtRunning()) {
                world_.handleKeyPress(code);
                state_ = State::RUNNING;
            } else if (isLevelEditor()) {
                switchMenu(Menu::Type::PAUSE, State::PAUSED);
            } else if (isPaused()) {
                state_ = State::LEVEL_EDITOR;
            }
            break;
        case sf::Keyboard::Key::R:
        case sf::Keyboard::Key::T:
        case sf::Keyboard::Key::A:
        case sf::Keyboard::Key::W:
        case sf::Keyboard::Key::D:
        case sf::Keyboard::Key::S:
        case sf::Keyboard::Key::Delete:
            if (isLevelEditor()) {
                levelEditor_.handleKeyPress(code);
            }
            break;
        default:
            break;
    } 
}

void GameModel::handleKeyRelease() {
    if (isLevelEditor()) {
        levelEditor_.handleKeyRelease();
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
    auto selectedItem = currentMenu_->getSelectedItem();
    if (selectedItem == 0) {
        auto& gameSelector = getMenu<GameSelector>(Menu::Type::GAME_SELECTOR);
        gameSelector.updateMenuItems();
        gameSelector.setScreen(GameSelector::Screen::GAME_SELECTOR);
        auto& pauseMenu = getMenu<Pause>(Menu::Type::PAUSE);
        pauseMenu.setPausedState(Pause::PausedState::RUNNING);
        pauseMenu.updateMenuItems();
        switchMenu(Menu::Type::GAME_SELECTOR, State::GAME_SELECTOR);
    } else if (selectedItem == 1) {
        currentMenu_->updateMusic(sf::SoundSource::Status::Stopped);
        auto& pauseMenu = getMenu<Pause>(Menu::Type::PAUSE);
        pauseMenu.setPausedState(Pause::PausedState::LEVEL_EDITOR);
        pauseMenu.updateMenuItems();
        state_ = State::LEVEL_EDITOR;
    } else if (selectedItem == 2) {
        switchMenu(Menu::Type::SETTINGS, State::SETTINGS);
    } else {
        state_ = State::QUIT;
    }
}

void GameModel::handleGameOverState() {
    auto &gameOverMenu = getMenu<GameOver>(Menu::Type::GAME_OVER);
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
        world_.resetLevel();
        state_ = State::RUNNING;
    } else if (selectedItem == nextLevelIndex) {
        auto &gameSelector = getMenu<GameSelector>(Menu::Type::GAME_SELECTOR);
        // Next Level
        world_.clearLevel();
        world_.loadLevel(gameSelector.getLevelSelector().getNextLevel().filename);
        state_ = State::RUNNING;
    } else if (selectedItem == mainMenuIndex) {
        // Main Menu
        switchMenu(Menu::Type::MAIN, State::MENU);
        currentMenu_->updateMusic(sf::SoundSource::Status::Playing);
    } else if (selectedItem == exitIndex) {
        // Exit
        state_ = State::QUIT;
    } 
}

void GameModel::handleGameSelectorScreenState() {
    auto &gameSelector = getMenu<GameSelector>(Menu::Type::GAME_SELECTOR);
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
            gameSelector.getLevelSelector().addNewLevel();
            gameSelector.getLevelSelector().updateLevel();
            gameSelector.setScreen(GameSelector::Screen::LEVEL_SELECTOR);
            break;
        case GameSelector::Item::BACK:
            switchMenu(Menu::Type::MAIN, State::MENU);
            break;
    }
}

void GameModel::handleUserSelectorScreenState() {
    auto &gameSelector = getMenu<GameSelector>(Menu::Type::GAME_SELECTOR);
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
    auto & gameSelector = getMenu<GameSelector>(Menu::Type::GAME_SELECTOR);
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
    const auto& gameSelector = getMenu<GameSelector>(Menu::Type::GAME_SELECTOR);
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
        switchMenu(Menu::Type::MAIN, State::MENU);
    }
}

void GameModel::handlePauseState() {
    switch (getMenu<Pause>(Menu::Type::PAUSE).getPausedState()) {
        case Pause::PausedState::RUNNING: {
            int selectedItem = currentMenu_->getSelectedItem();
            if (selectedItem == 0) {
                world_.handleKeyPress(sf::Keyboard::Key::P); // Unpause
                state_ = State::RUNNING;
            } else if (selectedItem == 1) {
                world_.resetLevel();
                state_ = State::RUNNING;
            } else if (selectedItem == 2) {
                switchMenu(Menu::Type::MAIN, State::MENU);
                updateView_ = true; // Center the view back to default
            } else if (selectedItem == 3) {
                state_ = State::QUIT;
            }
            break;
        }
        case Pause::PausedState::LEVEL_EDITOR: {
            int selectedItem = currentMenu_->getSelectedItem();
            if (selectedItem == 0) {
                state_ = State::LEVEL_EDITOR;
            } else if (selectedItem == 1) {
                switchMenu(Menu::Type::MAIN, State::MENU);
                updateView_ = true; // Center the view back to default
            } else if (selectedItem == 2) {
                state_ = State::QUIT;
            }
            break;
        }
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

void GameModel::handleTextEntered(const sf::Uint32& unicode) {
    if (state_ == State::GAME_SELECTOR) {
        currentMenu_->handleTextEntered(unicode);
    }
}

void GameModel::handleMouseMove(const sf::Vector2f& mousePosition) {
    if (isRunning()) {
        world_.handleMouseMove(mousePosition);
    } else if (isLevelEditor()) {
        levelEditor_.handleMouseMove(mousePosition);
    } else {
        currentMenu_->handleMouseMove(mousePosition);
    }
}

void GameModel::handleResize(const sf::RenderWindow& window) {
    for (auto& menu : menus_) {
        if (menu.second->getType() == Menu::Type::PAUSE) {
            menu.second->handleResize(window);
        } else {
            menu.second->handleResize();
        }
    }
    world_.handleResize();
    levelEditor_.handleResize();
}

void GameModel::handleMouseLeftClick(const sf::Vector2f& mousePosition, GameView& view) {
    if (isRunning()) {
        view.setUpdateHUD(true);
        world_.getCannon()->startLaunch();
    } else if (isLevelEditor()) {
        levelEditor_.handleMouseClick(mousePosition, view);
    } else {
        if (currentMenu_->handleMouseClick(mousePosition)) {
            setState();
        }
    }
}

void GameModel::handleMouseRelease(const sf::Mouse::Button& button, const sf::Vector2f& mousePosition) {
    if (button == sf::Mouse::Button::Left) {
        if (isRunning() && world_.getCannon()->isLaunching()) {
            launchBird();
        } else if (isLevelEditor()) {
            levelEditor_.handleMouseRelease();
        }
    }

}

void GameModel::draw(sf::RenderWindow& window) const {
    if (isRunning()) {
        world_.draw(window);
    } else if (isPausedAtRunning()) {
        world_.draw(window);
        currentMenu_->draw(window);
    } else if (isLevelEditor()) {
        levelEditor_.draw(window);
    } else if (isPaused()) {
        levelEditor_.draw(window);
        currentMenu_->draw(window);
    } else {
        currentMenu_->draw(window);
    }
}

bool GameModel::isRunning() const {
    return state_ == State::RUNNING;
}

bool GameModel::isPaused() const {
    return state_ == State::PAUSED;
}

bool GameModel::isPausedAtRunning() const {
    return isPaused() && getMenu<Pause>(Menu::Type::PAUSE).getPausedState() == Pause::PausedState::RUNNING;
}

bool GameModel::updateView() const {
    return updateView_;
}

void GameModel::setUpdateView(bool updateView) {
    updateView_ = updateView;
}

bool GameModel::isLevelEditor() const {
    return state_ == State::LEVEL_EDITOR;
}


LevelEditor& GameModel::getLevelEditor() {
    return levelEditor_;
}