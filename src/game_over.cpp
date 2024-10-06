#include "game_over.hpp"
#include "utils.hpp"
#include <vector>

GameOver::GameOver() : Menu(Menu::Type::GAME_OVER, 4) {
    // Load star textures
    starTextures_.resize(4);
    for (int i = 0; i < 4; ++i) {
        if (!utils::loadFromFile(starTextures_[i], "/assets/images/stars_" + std::to_string(i) + ".png")) {
            throw std::runtime_error("Failed to load texture file: assets/images/stars_" + std::to_string(i) + ".png");
        }
    }
    // Create star sprites
    starSprites_.resize(4);
    for (size_t i = 0; i < starSprites_.size(); ++i) {
        starSprites_[i].setTexture(starTextures_[i]);
        starSprites_[i].setScale(1, 0.7);
        auto textureRect = starSprites_[i].getTextureRect();
        starSprites_[i].setOrigin(textureRect.width / 2.f, textureRect.height / 2.f);
        auto y = 110/*90*/ + (starSprites_[i].getGlobalBounds().height / 2);
        starSprites_[i].setPosition(sf::Vector2f(SCREEN_CENTER.x, y));
    }
    // Create menu items
    std::vector<std::string> button_texts = { "Restart", "Next Level", "Main Menu", "Exit" };
    for (int i = 0; i < buttonAmount_; ++i) {
        menuItems_[i].setFont(font_);
        menuItems_[i].setString(button_texts[i]);
        menuItems_[i].setFillColor(sf::Color::White);
        menuItems_[i].setOutlineColor(sf::Color::Black);
        menuItems_[i].setOutlineThickness(5);
        menuItems_[i].setCharacterSize(75);
        // center the text
        float width = menuItems_[i].getGlobalBounds().width;
        menuItems_[i].setOrigin(width / 2, 0);
        menuItems_[i].setPosition(SCREEN_CENTER.x , (starSprites_[0].getGlobalBounds().height + 130) + i * 100);
        if (i == selectedItem_) {
            menuItems_[i].setFillColor(LIME_GREEN);
            menuItems_[i].setScale(1.1f, 1.1f);
        }
    }
    // Load wooden sign texture
    signImage_ = ResourceManager::getInstance().getTexture("/assets/images/wooden_sign.png");
    woodenSign_.setSize(sf::Vector2f(860, 860));
    woodenSign_.setTexture(&signImage_);
    woodenSign_.setOrigin(woodenSign_.getGlobalBounds().width / 2, woodenSign_.getGlobalBounds().height / 2);
    woodenSign_.setPosition(SCREEN_CENTER.x, SCREEN_CENTER.y - 30);

}

void GameOver::setScoreManager(Score* scoreManager) {
    scoreManager_ = scoreManager;
    scoreManager_->setPosition(sf::Vector2f(SCREEN_CENTER.x, 10));
    clock_.restart();
}

void GameOver::drawStars(int stars, sf::RenderWindow& window) const {
    sf::Time elapsed = clock_.getElapsedTime();
    if (elapsed.asSeconds() > 3 && stars == 3) {
        window.draw(starSprites_[3]);
    } else if (elapsed.asSeconds() > 2 && elapsed.asSeconds() < 3 && stars >= 2 || elapsed.asSeconds() > 2 && stars == 2) {
        window.draw(starSprites_[2]);
    } else if (elapsed.asSeconds() > 1 && elapsed.asSeconds() < 2 && stars >= 1 || elapsed.asSeconds() > 1 && stars == 1) {
        window.draw(starSprites_[1]);
    } else if (elapsed.asSeconds() > 0 && elapsed.asSeconds() < 1 || stars == 0) {
        window.draw(starSprites_[0]);
    }


}

void GameOver::draw(sf::RenderWindow& window) const {
    window.draw(background_);
    window.draw(woodenSign_);
    drawStars(scoreManager_->getStars(), window);
    scoreManager_->draw(window);
    for (auto menuItem : menuItems_)
    {
        window.draw(menuItem);
    }
};
