#include "game_over.hpp"
#include "utils.hpp"
#include <vector>

GameOver::GameOver() : Menu(Menu::Type::GAME_OVER) {
    const sf::Vector2f &SCREEN_CENTER = VIEW.getCenter();
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
        starSprites_[i].setPosition(sf::Vector2f(SCREEN_CENTER.x, SCREEN_CENTER.y - 280.f));
    }
    // Create menu items
    updateMenuItems();
}

const std::vector<std::string>& GameOver::getButtonNames() const {
    if (hasNextLevel_) {
        static const std::vector<std::string> buttonNames = { "Restart", "Next Level", "Main Menu", "Exit" };
        return buttonNames;
    }
    static const std::vector<std::string> buttonNames = { "Restart", "Main Menu", "Exit" };
    return buttonNames;
}

void GameOver::handleResize() {
    const sf::Vector2f& SCREEN_CENTER = VIEW.getCenter();
    Menu::handleResize();
    for (size_t i = 0; i < starSprites_.size(); ++i) {
        starSprites_[i].setPosition(sf::Vector2f(SCREEN_CENTER.x, SCREEN_CENTER.y - 280.f));
    }
    if (scoreManager_ != nullptr) {
        scoreManager_->setPosition(sf::Vector2f(SCREEN_CENTER.x, 10));
    }
}

void GameOver::setScoreManager(Score* scoreManager) {
    scoreManager_ = scoreManager;
    scoreManager_->setPosition(sf::Vector2f(VIEW.getCenter().x, 10));
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

bool GameOver::hasNextLevel() const {
    return hasNextLevel_;
}

void GameOver::setHasNextlevel(bool hasNextLevel) {
    hasNextLevel_ = hasNextLevel;
}
