#include "score.hpp"
#include "utils.hpp"


Score::Score() {
    font_ = ResourceManager::getInstance().getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
    text_.setFont(font_);
    text_.setCharacterSize(40);
    text_.setFillColor(sf::Color::White);
    text_.setOutlineColor(sf::Color::Black);
    text_.setOutlineThickness(2);
    text_.setString("Score: 0 High Score: " + std::to_string(highScore_));
}

void Score::updatePosition(const sf::RenderWindow& window) {
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
    stars_ = 0;
    text_.setScale(1.f, 1.f);
    text_.setString("Score: 0 High Score: " + std::to_string(highScore_));
}

void Score::draw(sf::RenderWindow& window) const {
    window.draw(text_);
}

void Score::setHighScores(const std::vector<HighScore>& highScores) {
    highScores_ = highScores;
}

void Score::updateHighScore(int highScore) {
    highScore_ = highScore;
    text_.setString("Score: " + std::to_string(currentScore_) + " High Score: " + std::to_string(highScore_));
}

bool Score::updateHighScores(const HighScore& highScore) {
    auto it = std::find_if(highScores_.begin(), highScores_.end(), [&highScore](const HighScore& hs){
        return hs.player == highScore.player && hs.score == highScore.score;
    });
    if (it == highScores_.end()) {
        highScores_.push_back(highScore);
        return true;
    }
    return false;
}

int Score::getCurrentScore() const {
    return currentScore_;
}

int Score::getHighScore() const {
    return highScore_;
}

int Score::getStars() const {
    return stars_;
}

void Score::setStars(int stars) {
    stars_ = stars;
}

void Score::setLevelEndText(const std::string& levelName) {
    text_.setString(levelName + ": Score: " + std::to_string(currentScore_) + " High Score: " + std::to_string(highScore_));
    text_.setScale(1.2f, 1.2f);
}

const std::vector<HighScore>& Score::getHighScores() const {
    return highScores_;
}