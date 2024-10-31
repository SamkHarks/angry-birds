#include "world.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>


World::World() : gravity_(0.0f, -9.8f), levelLoader_(*this), scoreManager_() {
    world_ = new b2World(gravity_);
    cannon_ = new Cannon();
    background_image_ = ResourceManager::getInstance().getTexture("/assets/images/background.jpg");
    background_.setSize(sf::Vector2f(VIEW_WIDTH * 2, VIEW_HEIGHT * 1.8f));
    background_.setTexture(&background_image_);
    background_.setPosition(0,-VIEW_HEIGHT+200);
    
}

World::~World() {
    for (auto object : objects_) {
        delete object;
    }
    for (auto bird : birds_) {
        delete bird;
    }
    delete cannon_;
    delete world_;
}

void World::addObject(Object *object) {
    if (object->getType() == Object::Type::Bird) {
        birds_.push_back(dynamic_cast<Bird *>(object));
    } else {
        objects_.push_back(object);
    }
}


void World::loadLevel(const std::string& filename) {
    levelLoader_.loadLevel(filename);
}

void World::saveHighScore(int score) {
    if (score == 0) {
        return;
    }
    HighScore highScore;
    highScore.player = player_.name;
    highScore.score = score;
    if (scoreManager_.updateHighScores(highScore)) {
        levelLoader_.saveHighScores(scoreManager_.getHighScores(), fileName_);
    }
    if (score > scoreManager_.getHighScore()) {
        scoreManager_.updateHighScore(score);
    }
}

int World::getStars() const {
    int pigsLeft = getRemainingPigCount();
    int birdsLeft = getRemainingBirdCount();
    float maxScore = (totalPigCount_ + totalBirdCount_- 1) * 1000.f;
    int score = scoreManager_.getCurrentScore();
    float scoreRatio = score / maxScore;
    if (scoreRatio >= 0.9) {
        return 3;
    } else if (scoreRatio >= 0.6) {
        return 2;
    } else if (scoreRatio >= 0.5) {
        return 1;
    } else {
        return 0;
    }

}

int World::getRemainingPigCount() const {
    int count = 0;
    for (auto object : objects_) {
        if (object->getType() == Object::Type::Pig) {
            count++;
        }
    }
    return count;
}

int World::getRemainingBirdCount() const {
    auto count = birds_.size();
    if (count > 0) {
        Bird *bird = birds_.front();
        if (bird->isLaunched()) {
            count--;
        }
    }
    return count;
}

void World::step() {
    world_->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
}

b2World* World::getWorld() {
    return world_;
}

std::list<Object *> World::getObjects() {
    return objects_;
}

Bird* World::GetBird() {
    if (birds_.empty()) {
        return nullptr;
    }
    return birds_.front();
}

const Bird* World::GetBird() const {
    if (birds_.empty()) {
        return nullptr;
    }
    return birds_.front();
}

void World::draw(sf::RenderWindow &window) const {
    window.draw(background_);
    scoreManager_.draw(window);
    drawRemainingCounts(window);
    for (auto object : objects_) {
        window.draw(object->getSprite());
    }
    const Bird* bird = GetBird();
    if (bird != nullptr && bird->isLaunched()) {
        window.draw(bird->getSprite());
    }
    cannon_->draw(window);
}

void World::removeObject(Object *object) {
    world_->DestroyBody(object->getBody());
    objects_.remove(object);
    delete object;
}

void World::removeBird() {
    if (!birds_.empty()) {
        Bird* bird = birds_.front();
        world_->DestroyBody(bird->getBody());
        birds_.pop_front();
        delete bird;
    }
}

Cannon* World::getCannon() {
    return cannon_;
}

void World::resetBird() {
    Bird* bird = GetBird();
    if (bird != nullptr) {
        b2Body* body = bird->getBody();
        body->SetTransform(BIRD_INITIAL_POSITION, 0);
        body->SetLinearVelocity(b2Vec2_zero);
        body->SetAngularVelocity(0);
        body->SetAwake(false);
        bird->setLaunched(false);
    }
}

void World::clearLevel() {
    // Destroy all bodies in the Box2D world
    for (b2Body* body = world_->GetBodyList(); body != nullptr; ) {
        b2Body* nextBody = body->GetNext();
        world_->DestroyBody(body);
        body = nextBody;
    }

    // Clear the objects
    for (auto object : objects_) {
        delete object;
    }
    objects_.clear();

    // Clear the birds
    for (auto bird : birds_) {
        delete bird;
    }
    birds_.clear();

    // Reset cannon
    cannon_->reset();

    // reset score
    scoreManager_.reset();
};

void World::resetLevel() {
    // Clear the level
    clearLevel();
    // Reload the level
    loadLevel(fileName_);
}

void World::updateScore(int score) {
    scoreManager_.update(score);
}

Score& World::getScore() {
    return scoreManager_;
}

void World::drawRemainingCounts(sf::RenderWindow &window) const {
    for (const auto& sfObject : sfObjects_) {
        window.draw(sfObject.sprite);
        window.draw(sfObject.text);
    }
}

void World::updateRemainingCountPositions(sf::RenderWindow& window) {
    int offset = cannon_->getTextWidth() + 40;
    for (auto& sfObject : sfObjects_) {
        sfObject.sprite.setPosition(window.mapPixelToCoords(sf::Vector2i(offset, 10)));
        sfObject.text.setPosition(window.mapPixelToCoords(sf::Vector2i(offset + 20, 45)));
        offset += 60;
    }
}

void World::updateRemainingCounts(char type) {
    for (auto& sfObject : sfObjects_) {
        if (sfObject.type == type) {
            sfObject.count--;
            sfObject.text.setString(std::to_string(sfObject.count));
            break;
        }
    }
}

const std::string& World::getLevelName() const {
    return levelName_;
}

void World::setPlayer(const Player& player) {
    player_ = player;
}

int World::getAliveBirdCount() const {
    return birds_.size();
}

int World::getLevelIndex() const {
    return levelIndex_;
}

const Player& World::getPlayer() const {
    return player_;
}

bool World::updatePlayer() {
    int score = scoreManager_.getCurrentScore();
    int stars = scoreManager_.getStars();
    bool hasUpdated = false;
    // push new score if score doesn't exist or if new score is higher
    if (player_.highScores.size() <= levelIndex_) {
        player_.highScores.resize(levelIndex_ + 1, 0);
        player_.highScores[levelIndex_] = score;
        hasUpdated = true;
    } else if (player_.highScores[levelIndex_] < score) {
        player_.highScores[levelIndex_] = score;
        hasUpdated = true;
    }
    // push new stars if stars doesn't exist or if new stars is higher
    if (player_.stars.size() <= levelIndex_) {
        player_.stars.resize(levelIndex_ + 1, 0);
        player_.stars[levelIndex_] = stars;
        hasUpdated = true;
    } else if (player_.stars[levelIndex_] < stars) {
        player_.stars[levelIndex_] = stars;
        hasUpdated = true;
    }
    return hasUpdated;
}

bool World::isWorldSettled() const {
    bool isLevelCleared = getRemainingPigCount() == 0 || getAliveBirdCount() == 0;
    bool isSettled = false;
    if (isLevelCleared) {
        const Bird *bird = GetBird();
        isSettled = (bird == nullptr || !bird->isMoving());
        // Early return if bird is not settled
        if (!isSettled) {
            return isSettled;
        }
        // Check if other objects are settled
        for (auto object : objects_) {
            if (object->isMoving()) {
                isSettled = false;
                break;
            }
        }
        
    }
    return isSettled;
}