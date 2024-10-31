#ifndef SCORE_HPP
#define SCORE_HPP

#include <SFML/Graphics.hpp>
#include "resource_manager.hpp"

struct HighScore {
    std::string player;
    int score;
};

class Score {
public:
    Score();
    void update(int score);
    void reset();
    void draw(sf::RenderWindow& window) const;
    void updateHighScore(int highScore);
    bool updateHighScores(const HighScore& highScore);
    void setHighScores(const std::vector<HighScore>& highScores);
    const std::vector<HighScore>& getHighScores() const;
    void updatePosition(const sf::RenderWindow& window);
    void setPosition(const sf::Vector2f &position);
    int getCurrentScore() const;
    int getHighScore() const;
    int getStars() const;
    void setStars(int stars);
    void setLevelEndText(const std::string& levelName);
private:
    int currentScore_ = 0;
    int highScore_ = 0;
    int stars_ = 0;
    std::vector<HighScore> highScores_;
    sf::Text text_;
    sf::Font font_;
};

#endif // SCORE_HPP