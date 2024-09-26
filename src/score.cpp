#include "score.hpp"
#include "utils.hpp"


Score::Score() {
    font_ = ResourceManager::getInstance().getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
    text_.setFont(font_);
    text_.setCharacterSize(30);
    text_.setFillColor(sf::Color::Black);

    text_.setPosition(VIEW_WIDTH - 400, 10);
    text_.setString("Score: 0 Hihg Score: " + std::to_string(highScore_));
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