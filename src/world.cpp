#include "world.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>


World::World() : gravity_(0.0f, -9.8f), levelLoader_(*this) {
    world_ = new b2World(gravity_);
    cannon_ = new Cannon(
            "/assets/images/cannon_barrel.png",
            "/assets/images/cannon_wheel.png",
            "/assets/fonts/BerkshireSwash-Regular.ttf"
        );
    if(!utils::loadFromFile(background_image_, "/assets/images/background.jpg")) {
        std::cerr << "Failed to load image for the world" << std::endl;
    } else {
        background_.setSize(sf::Vector2f(VIEW_WIDTH, VIEW_HEIGHT));
        background_.setTexture(&background_image_);
        background_.setPosition(0,0);
    }
    
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

std::tuple<int, float> World::getScoreAndStars() const {
    int pigsLeft = getRemainingPigCount();
    int birdsLeft = getRemainingBirdCount();
    float maxScore = (totalPigCount_ + totalBirdCount_) * 1000.f;
    float score = 1000.f * ((totalPigCount_ - pigsLeft) + birdsLeft);
    float scoreRatio = score / maxScore;
    if (scoreRatio >= 0.9) {
        return std::make_tuple(3, score);
    } else if (scoreRatio >= 0.6) {
        return std::make_tuple(2, score);
    } else if (scoreRatio >= 0.5) {
        return std::make_tuple(1, score);
    } else {
        return std::make_tuple(0, score);
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
    for (auto object : objects_) {
        window.draw(object->getSprite());
    }
    const Bird* bird = GetBird();
    if (bird != nullptr && bird->isLaunched()) {
        const sf::Sprite& sprite = bird->getSprite();
        window.draw(sprite);
    }
    cannon_->draw(window);
}

void World::debugDraw() const {
    // Loop through all objects and draw their Box2D shapes
    sf::Color sfmlRed = sf::Color::Red;
    sf::Color sfmlGreen = sf::Color::Green;
    b2Color box2DRed(sfmlRed.r / 255.0f, sfmlRed.g / 255.0f, sfmlRed.b / 255.0f);
    b2Color box2DGreen(sfmlGreen.r / 255.0f, sfmlGreen.g / 255.0f, sfmlGreen.b / 255.0f);
    for (auto object : objects_) {
        auto body = object->getBody(); // Assuming this method exists and returns a b2Body*
        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
            
            // Depending on the type of the fixture, call the appropriate SFMLDebugDraw method
            if (f->GetType() == b2Shape::e_polygon) {
                // Cast the shape to a polygon shape to access vertices
                b2PolygonShape* polygonShape = static_cast<b2PolygonShape*>(f->GetShape());
                debugDraw_->DrawPolygon(polygonShape->m_vertices, polygonShape->m_count, box2DRed);
            } else if (f->GetType() == b2Shape::e_circle) {
                // Cast the shape to a circle shape to access center and radius
                b2CircleShape* circleShape = static_cast<b2CircleShape*>(f->GetShape());
                // Calculate the global position of the circle's center
                b2Vec2 center = body->GetWorldPoint(circleShape->m_p);
                debugDraw_->DrawCircle(center, circleShape->m_radius, box2DRed);
            }
            // Add handling for other shape types (e.g., circle) as needed
        }
    }

    // Loop through birds if they are handled separately
    for (auto bird : birds_) {
    //const Bird* bird = GetBird(); // Assuming this method exists and returns a single bird for simplicity
        const auto birdBody = bird->getBody(); // Assuming getBody() returns a b2Body*
        for (const b2Fixture* f = birdBody->GetFixtureList(); f; f = f->GetNext()) {
            // Assuming birds are represented with a different color or shape
                if (f->GetType() == b2Shape::e_circle) {
                    const b2CircleShape* circleShape = static_cast<const b2CircleShape*>(f->GetShape());
                    // Get the radius of the circle
                    float radius = circleShape->m_radius;
                    // Calculate the global position of the circle's center
                    b2Vec2 center = birdBody->GetWorldPoint(circleShape->m_p);
                    // Convert Box2D position to SFML position (if necessary, depending on your coordinate system)
                    sf::Vector2f sfCenter(center.x, center.y);
                    // Convert Box2D radius to SFML radius (if necessary, depending on your scale factor)
                    float sfRadius = radius;
                    // Draw the circle with SFMLDebugDraw
                    debugDraw_->DrawCircle(center, sfRadius, box2DGreen);
                }
        }
    }
   
}

void World::setDebugDraw(SFMLDebugDraw* debugDraw) {
    this->debugDraw_ = debugDraw;
};

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

void World::setIsSettled(bool isSettled) {
    isSettled_ = isSettled;
}

bool World::getIsSettled() const {
    return isSettled_;
}

void World::resetLevel() {
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

    // set world to not settled
    isSettled_ = false;

    // Reload the level
    loadLevel(fileName_);
}