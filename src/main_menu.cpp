#include "main_menu.hpp"
#include <vector>
#include "utils.hpp"
#include "resource_manager.hpp"


MainMenu::MainMenu() : Menu(Menu::Type::MAIN, 3), levelSelector_(), userSelector_() {
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
       userSelector_.draw(window);
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

MainMenu::Screen MainMenu::getScreen() const {
    return screen_;
};

void MainMenu::setScreen(MainMenu::Screen screen) {
    screen_ = screen;
};

LevelSelector& MainMenu::getLevelSelector() {
    return levelSelector_;
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
        case MainMenu::Screen::USER_SELECTOR: {
            userSelector_.handleMouseMove(mousePosition);
            break;
        }
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
        case MainMenu::Screen::USER_SELECTOR:
            return userSelector_.handleMouseClick(mousePosition);
        default:
            break;
    }
    return false;
};

UserSelector& MainMenu::getUserSelector() {
    return userSelector_;
};