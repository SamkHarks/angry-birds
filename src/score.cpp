#include "score.hpp"
#include "utils.hpp"


Score::Score() {
    font_ = ResourceManager::getInstance().getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
    text_.setFont(font_);
    text_.setCharacterSize(40);
    text_.setFillColor(sf::Color::Black);
    text_.setOutlineColor(sf::Color::White);
    text_.setOutlineThickness(2);
    text_.setString("Score: 0 High Score: " + std::to_string(highScore_));
}

void Score::updatePosition(sf::RenderWindow& window) {
    sf::FloatRect textBounds = text_.getGlobalBounds();
    float viewWidth = window.getView().getSize().x;
    float xPosition = viewWidth - textBounds.width - 20;
    text_.setPosition(window.mapPixelToCoords(sf::Vector2i(xPosition, 10)));
}

void Score::setPosition(const sf::Vector2f& position) {
    sf::FloatRect textBounds = text_.getGlobalBounds();
    // Center the text
    text_.setPosition(position.x - (textBounds.width / 2.f), position.y);
}

void Score::update(int score) {
    currentScore_ += score;
    text_.setString("Score: " + std::to_string(currentScore_) + " High Score: " + std::to_string(highScore_));
}

void Score::reset() {
    currentScore_ = 0;
    text_.setString("Score: 0");
}

void Score::draw(sf::RenderWindow& window) const {
    window.draw(text_);
}

void Score::updateHighScore(int highScore) {
    highScore_ = highScore;
    text_.setString("Score: " + std::to_string(currentScore_) + " High Score: " + std::to_string(highScore_));
}