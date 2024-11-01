#include "main_menu.hpp"
#include <vector>
#include "utils.hpp"
#include "resource_manager.hpp"


MainMenu::MainMenu() : Menu(Menu::Type::MAIN, 3), levelSelector_(), userSelector_() {
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

    setTitle("AngryBirds", 680.f, 340);
    backgroundMusic_.play();
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