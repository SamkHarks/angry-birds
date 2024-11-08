#include "world.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>


World::World() : gravity_(0.0f, -9.8f), levelLoader_(*this), scoreManager_() {
    world_ = new b2World(gravity_);
    cannon_ = new Cannon();
    int height = VIEW.getHeight();
    background_image_ = ResourceManager::getInstance().getTexture("/assets/images/background.jpg");
    background_.setSize(sf::Vector2f(VIEW.getWidth() * 2, height * 1.8f));
    background_.setTexture(&background_image_);
    background_.setPosition(0,-height+200);
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
    // Only update high scores if the player is set
    if (auto player = player_.lock()) {
        HighScore highScore;
        highScore.player = player->name;
        highScore.score = score;
        if (scoreManager_.updateHighScores(highScore)) {
            levelLoader_.saveHighScores(scoreManager_.getHighScores(), fileName_);
        }
        if (score > scoreManager_.getHighScore()) {
            scoreManager_.updateHighScore(score);
        }
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

void World::updateRemainingCountPositions(const sf::RenderWindow& window) {
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

void World::setPlayer(const std::shared_ptr<Player>& player) {
    if (player == player_.lock()) {
        return;
    }
    player_ = player;
}

int World::getAliveBirdCount() const {
    return birds_.size();
}

int World::getLevelIndex() const {
    return levelIndex_;
}

bool World::updatePlayer() {
    bool hasUpdated = false;
    if (auto player = player_.lock()) {
        int score = scoreManager_.getCurrentScore();
        int stars = scoreManager_.getStars();
        // push new score if score doesn't exist or if new score is higher
        if (player->highScores.size() <= levelIndex_) {
            player->highScores.resize(levelIndex_ + 1, 0);
            player->highScores[levelIndex_] = score;
            hasUpdated = true;
        } else if (player->highScores[levelIndex_] < score) {
            player->highScores[levelIndex_] = score;
            hasUpdated = true;
        }
        // push new stars if stars doesn't exist or if new stars is higher
        if (player->stars.size() <= levelIndex_) {
            player->stars.resize(levelIndex_ + 1, 0);
            player->stars[levelIndex_] = stars;
            hasUpdated = true;
        } else if (player->stars[levelIndex_] < stars) {
            player->stars[levelIndex_] = stars;
            hasUpdated = true;
        }
        // update levels completed
        if (player->levelsCompleted <= levelIndex_ && stars > 0) {
            player->levelsCompleted = levelIndex_ + 1;
            hasUpdated = true;
        }
    }
    return hasUpdated;
}

bool World::isSettled() const {
    bool isLevelCleared = getRemainingPigCount() == 0 || getAliveBirdCount() == 0;
    if (!isLevelCleared) {
        return false;  // Early exit if level is not cleared
    }

    const Bird* bird = GetBird();
    if (bird != nullptr && bird->isMoving()) {
        return false;  // Bird is still moving
    }

    // Check if any objects are still moving
    for (auto object : objects_) {
        if (object->isMoving()) {
            return false;
        }
    }

    return true;  // All conditions met, world is settled
}

void World::handleResize() {
    // Original dimensions of the background image
    float originalWidth = WORLD_WIDTH;
    float originalHeight = WORLD_HEIGHT;

    // Calculate the scale factors based on the new window size
    float scaleX = (2 * VIEW.getWidth()) / originalWidth;
    float scaleY = (1.8f * VIEW.getHeight()) / originalHeight;

    // Choose the smaller of the two scale factors
    float scaleFactor = std::min(scaleX, scaleY);

    // Apply the scaling to the background (this scales both axes proportionally)
    background_.setScale(scaleFactor, scaleFactor);
    // Resize cannon properties
    cannon_->handleResize();
}

void World::handleMouseMove(const sf::Vector2f& mousePosition) {
    cannon_->handleMouseMove(mousePosition);
}