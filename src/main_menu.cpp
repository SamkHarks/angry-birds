#include "main_menu.hpp"
#include <vector>
#include "utils.hpp"
#include "resource_manager.hpp"


MainMenu::MainMenu() : Menu(Menu::Type::MAIN, 3), levelSelector_() {
    // Load wooden sign texture
    signImage_ = ResourceManager::getInstance().getTexture("/assets/images/wooden_sign.png");
    woodenSign_.setSize(sf::Vector2f(860, 860));
    woodenSign_.setTexture(&signImage_);
    woodenSign_.setOrigin(woodenSign_.getGlobalBounds().width / 2, woodenSign_.getGlobalBounds().height / 2);
    woodenSign_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y - 30);
    // Create menu items
    std::vector<std::string> button_texts = { "Play", "Settings", "Exit" };
    for (int i = 0; i < buttonAmount_; ++i) {
        menuItems_[i].setFont(font_);
        menuItems_[i].setString(button_texts[i]);
        menuItems_[i].setFillColor(sf::Color::White);
        menuItems_[i].setOutlineColor(sf::Color::Black);
        menuItems_[i].setOutlineThickness(5);
        menuItems_[i].setCharacterSize(80);
        // center the text
        sf::FloatRect textBounds = menuItems_[i].getGlobalBounds();
        menuItems_[i].setOrigin(textBounds.width / 2, textBounds.height / 2);
        menuItems_[i].setPosition(SCREEN_CENTER.x, 350 + i * 100);
        if (i == selectedItem_) {
            menuItems_[i].setFillColor(LIME_GREEN);
            menuItems_[i].setScale(1.1f, 1.1f);
        }
    }

    // Define the circular path segment
    float radius = 680.f;
    sf::Vector2f center(SCREEN_CENTER.x, SCREEN_CENTER.y+340);
    std::string title = "AngryBirds";
    float startAngle = 270 - 20.f;
    float endAngle = 270 + 20.f;

    // Calculate the total width of the title
    float totalWidth = 0.0f;
    for (char c : title) {
        sf::Text tempText;
        tempText.setFont(font_);
        tempText.setString(c);
        tempText.setCharacterSize(80);
        tempText.setOutlineThickness(5);
        totalWidth += tempText.getGlobalBounds().width;
    }

    // Create title characters
    title_.resize(title.size());
    float currentAngle = startAngle;
    for (size_t i = 0; i < title.size(); ++i) {
        title_[i].setFont(font_);
        title_[i].setString(title[i]);
        title_[i].setFillColor(sf::Color::White);
        title_[i].setOutlineColor(sf::Color::Black);
        title_[i].setOutlineThickness(5);
        title_[i].setCharacterSize(80);

        // Calculate the width of the current character
        float charWidth = title_[i].getGlobalBounds().width;

        // Calculate the angle for the current character
        
        float angle = utils::DegreesToRadians(currentAngle);
        float x = center.x + radius * std::cos(angle);
        float y = center.y + radius * std::sin(angle);

        // Set the position and rotation
        title_[i].setPosition(x, y);
        title_[i].setRotation(currentAngle + 90); // Adjust rotation

        // Update the current angle based on the width of the character
        currentAngle += (charWidth / totalWidth) * (endAngle - startAngle);
    }

    // set prompt text
    promptText_.setFont(font_);
    promptText_.setString("Add player:");
    promptText_.setFillColor(sf::Color::White);
    promptText_.setOutlineColor(sf::Color::Black);
    promptText_.setOutlineThickness(5);
    promptText_.setCharacterSize(80);
    sf::FloatRect textBounds = promptText_.getGlobalBounds();
    promptText_.setOrigin(textBounds.width / 2, textBounds.height / 2);
    promptText_.setPosition(SCREEN_CENTER.x, 400);
    // set player text
    playerText_.setFont(font_);
    playerText_.setFillColor(sf::Color::White);
    playerText_.setOutlineColor(sf::Color::Black);
    playerText_.setOutlineThickness(5);
    playerText_.setCharacterSize(70);
    playerText_.setString("");
    textBounds = playerText_.getGlobalBounds();
    playerText_.setOrigin(textBounds.width / 2, 0);
    playerText_.setPosition(SCREEN_CENTER.x, 470);

    // Initialize caret
    caret_.setSize(sf::Vector2f(3, playerText_.getCharacterSize()*1.3));
    caret_.setFillColor(sf::Color::White);
    caret_.setOutlineColor(sf::Color::Black);
    caret_.setOutlineThickness(5);
    caret_.setPosition(
        SCREEN_CENTER.x,
        playerText_.getPosition().y
    );
};

void MainMenu::draw(sf::RenderWindow& window) const {
    window.draw(background_);
    window.draw(woodenSign_);
    // Draw the title characters
    for (const auto& character : title_) {
        window.draw(character);
    }
    // Draw player selection screen
    if (screen_ == MainMenu::Screen::USER_SELECTOR) {
        window.draw(promptText_);
        window.draw(playerText_);
        if (((int) caretClock_.getElapsedTime().asSeconds() % 2) == 1) {
            window.draw(caret_);
        }
    // Draw level selector screen
    } else if (screen_ == MainMenu::Screen::LEVEL_SELECTOR) {
       levelSelector_.draw(window);
    // Draw main menu screen
    } else {
        // Draw menu items
        for (auto menuItem : menuItems_) {
            window.draw(menuItem);
        }
    }

};

bool MainMenu::setPlayer() {
    std::string playerName = playerText_.getString();
    if (playerName.length() >= 3) {
        player_ = playerName;
        return true;
    }
    return false;
};

bool MainMenu::isPlayerSet() const {
    return !player_.empty();
};


std::string MainMenu::getPlayer() const {
    return player_;
};

void MainMenu::setPromptText(const std::string& text) {
    promptText_.setString(text);
    sf::FloatRect textBounds = promptText_.getGlobalBounds();
    promptText_.setOrigin(textBounds.width / 2, textBounds.height / 2);
};

void MainMenu::setPlayerText(const std::string& text) {
    playerText_.setString(text);
    sf::FloatRect textBounds = playerText_.getGlobalBounds();
    playerText_.setOrigin(textBounds.width / 2, 0);
    playerText_.setPosition(SCREEN_CENTER.x, 470);
    caret_.setPosition(
        playerText_.getPosition().x + textBounds.width / 2,
        playerText_.getPosition().y
    );
};


std::string MainMenu::getPlayerText() const {
    return playerText_.getString();
}

MainMenu::Screen MainMenu::getScreen() const {
    return screen_;
};

void MainMenu::setScreen(MainMenu::Screen screen) {
    screen_ = screen;
};

LevelSelector& MainMenu::getLevelSelector() {
    return levelSelector_;
};

void MainMenu::handleMenuSelection(sf::Keyboard::Key key) {
    switch (screen_) {
        case MainMenu::Screen::MAIN:
            if (key == sf::Keyboard::Key::Up) {
                setSelectedItem(selectedItem_ - 1);
            } else if (key == sf::Keyboard::Key::Down) {
                setSelectedItem(selectedItem_ + 1);
            }
            break;
        case MainMenu::Screen::USER_SELECTOR:
            break;
        case MainMenu::Screen::LEVEL_SELECTOR:
            if (key == sf::Keyboard::Key::Left || key == sf::Keyboard::Key::Right) {
                levelSelector_.setSelectedItem(getLevelSelector().getSelectedItem() == LevelSelector::Item::PREV
                    ? LevelSelector::Item::NEXT
                    : LevelSelector::Item::PREV
                );
            } else if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::Down) {
                levelSelector_.setSelectedItem(getLevelSelector().getSelectedItem() == LevelSelector::Item::LEVEL
                    ? LevelSelector::Item::BACK
                    : LevelSelector::Item::LEVEL
                );
            }
    }
};

void MainMenu::handleMouseMove(sf::Vector2f mousePosition) {
    switch (screen_) {
        case MainMenu::Screen::MAIN: {
            this->Menu::handleMouseMove(mousePosition);
            break;
        }
        case MainMenu::Screen::LEVEL_SELECTOR:
            levelSelector_.handleMouseMove(mousePosition);
            break;
        default:
            break;
    }
};

bool MainMenu::handleMouseClick(sf::Vector2f mousePosition) {
    switch (screen_) {
        case MainMenu::Screen::MAIN:
            return this->Menu::handleMouseClick(mousePosition);
        case MainMenu::Screen::LEVEL_SELECTOR:
            return levelSelector_.handleMouseClick(mousePosition);
        default:
            break;
    }
    return false;
};