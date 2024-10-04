#include "game_over.hpp"
#include "utils.hpp"
#include <vector>

GameOver::GameOver() : Menu(Menu::Type::GAME_OVER, 4) {
    std::vector<std::string> button_texts = { "Restart", "Next Level", "Main Menu", "Exit" };
    for (int i = 0; i < buttonAmount_; ++i) {
        menuItems_[i].setFont(font_);
        menuItems_[i].setString(button_texts[i]);
        menuItems_[i].setFillColor(sf::Color::Black);
        menuItems_[i].setCharacterSize(80);
        // center the text
        sf::FloatRect textBounds = menuItems_[i].getGlobalBounds();
        menuItems_[i].setOrigin(textBounds.width / 2, textBounds.height / 2);
        menuItems_[i].setPosition(SCREEN_CENTER.x , 200 + i * 100);
        if (i == selectedItem_) {
            menuItems_[i].setOutlineColor(sf::Color::White);
            menuItems_[i].setOutlineThickness(5);
        }
    }
}

void GameOver::setScoreManager(Score* scoreManager) {
    scoreManager_ = scoreManager;
    scoreManager_->setPosition(sf::Vector2f(SCREEN_CENTER.x, 100));
}

void GameOver::draw(sf::RenderWindow& window) const {
    window.draw(background_);
    scoreManager_->draw(window);
    for (auto menuItem : menuItems_)
    {
        window.draw(menuItem);
    }
};
