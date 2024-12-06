#include "bird.hpp"
#include <iostream>
#include "utils.hpp"

Bird::Bird(b2Body *body, std::string textureFile, Bird::Type birdType, float radius) : 
Object(body, Object::Type::Bird, textureFile, 100.f, true), birdType_(birdType), radius_(radius) {
    int width = sprite_.getTextureRect().width;
    int height = sprite_.getTextureRect().height;
    sprite_.setScale(radius * SCALE * 2.f / (1.f * width), radius * SCALE * 2.f / (1.f * height));
    sprite_.setOrigin(width / 2.f, height / 2.f);
}

MiniBird::MiniBird(b2Body *body, float radius) :
Object(body, Object::Type::MiniBird, "/assets/images/blue_bird.png", 50.f, true) {
    int width = sprite_.getTextureRect().width;
    int height = sprite_.getTextureRect().height;
    sprite_.setScale(radius * SCALE * 2.f / (1.f * width), radius * SCALE * 2.f / (1.f * height));
    sprite_.setOrigin(width / 2.f, height / 2.f);
}

Bird::Type Bird::getBirdType() const {
    return birdType_;
}

void Bird::draw(sf::RenderWindow &window) const {
    if (isLaunched_) {
        window.draw(sprite_);
    }
}

bool Bird::isLaunched() const {
    return isLaunched_;
}

void Bird::setLaunched(bool launched) {
    pressClock_.restart();
    isLaunched_ = launched;
}

void Bird::update() {
    if (isLaunched_) {
        Object::update();
    }
}

RedBird::RedBird(b2Body *body, float radius) : Bird(body, "/assets/images/red_bird.png", Bird::Type::Red, radius) {}

char RedBird::getTypeAsChar() const {
    return 'R';
}

BlueBird::BlueBird(b2Body *body, float radius) : Bird(body, "/assets/images/blue_bird.png", Bird::Type::Blue, radius) {}

char BlueBird::getTypeAsChar() const {
    return 'L';
}

void BlueBird::update() {
    Bird::update();
    if (isPowerUsed_) {
        for (std::list<std::unique_ptr<MiniBird>>::iterator it = miniBirds_.begin(); it != miniBirds_.end();) {
            auto& miniBird = *it;
            if (miniBird->shouldRemove()) {
                it = removeDestroyedMiniBird(it);
            } else {
                miniBird->update(); 
                it++;
            }
        }
    }
}

GreenBird::GreenBird(b2Body *body, float radius) : Bird(body, "/assets/images/green_bird.png", Bird::Type::Green, radius) {}

char GreenBird::getTypeAsChar() const {
    return 'G';
}

bool Bird::shouldRemove() const {
    if (isDestroyed()) {
        return true;
    }
    if (!isLaunched_) { 
        return false;
    }
   	if (this->isOutOfBounds()) {
		return true;
	}
    if (getActiveTime() >= 7.f) {
        return true;
    }
    return false;
}

void Bird::setIsPaused(bool paused) {
    if (paused) {
        pauseClock_.restart();
    } else {
        pausedTime_ += pauseClock_.getElapsedTime().asSeconds();
    }
    isPaused_ = paused;
}

float Bird::getActiveTime() const {
    return pressClock_.getElapsedTime().asSeconds() - pausedTime_;
}

bool Bird::getIsPaused() const {
    return isPaused_;
}

void RedBird::usePower() {
    if (isPowerUsed_ || !isLaunched_) {
        return;
    }
    float power = 10.f;
    b2Vec2 force = b2Vec2(0, -power); // Apply force downwards
    body_->ApplyLinearImpulse(force, body_->GetPosition(), true);
    body_->SetBullet(true);
    isPowerUsed_ = true;
}

void BlueBird::createMiniBird(b2Vec2 position) {
    b2BodyDef bodyDef;
    bodyDef.position = position;
    bodyDef.angle = body_->GetAngle();
    bodyDef.angularVelocity = body_->GetAngularVelocity();
    bodyDef.linearVelocity = body_->GetLinearVelocity();
    bodyDef.angularDamping = body_->GetAngularDamping();
    bodyDef.linearDamping = body_->GetLinearDamping();
    bodyDef.gravityScale = body_->GetGravityScale();
    bodyDef.type = b2_dynamicBody;
    bodyDef.awake = true;

    // Create the body in the Box2D world
    b2Body* new_body = body_->GetWorld()->CreateBody(&bodyDef);

    // Create the shape and fixture
    b2CircleShape circle;
    circle.m_p = b2Vec2_zero;
    circle.m_radius = radius_ * 0.75f;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 1;
    fixtureDef.friction = 1;
    fixtureDef.restitution = 0.4;

    auto miniBird = std::make_unique<MiniBird>(new_body, circle.m_radius);
    fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(miniBird.get());
    // Create the fixture on the new body
    new_body->CreateFixture(&fixtureDef);
    // Store the MiniBird in the vector
    miniBirds_.push_back(std::move(miniBird));
}

void BlueBird::destroyMiniBirds() {
    for (auto& miniBird : miniBirds_) {
        miniBird->getBody()->GetWorld()->DestroyBody(miniBird->getBody());
    }
    miniBirds_.clear();
}

void BlueBird::usePower() {
    if (isPowerUsed_ || !isLaunched_) {
        return;
    }
    //TODO: Implement power
    isPowerUsed_ = true;
    b2Vec2 position = body_->GetPosition();
    float yPositions[3] = {position.y - 2, position.y, position.y + 2};
    for (int i = 0; i < 3; i++) {
        b2Vec2 newPosition;
        newPosition.y = yPositions[i];
        newPosition.x = i == 1 ? position.x + 2 : position.x;
        createMiniBird(newPosition);
    }
    float yImpulses[3] = {-0.1, 0, 0.1};
    int i = 0;
    for (auto& miniBird : miniBirds_) {
        miniBird->getBody()->ApplyLinearImpulse(b2Vec2(1, yImpulses[i]), miniBird->getBody()->GetPosition(), true);
        i++;
    }
}

void BlueBird::draw(sf::RenderWindow &window) const {
    Bird::draw(window);
    if (isPowerUsed_) {
        for (auto& miniBird : miniBirds_) {
            miniBird->draw(window);
        }
    }
}

std::list<std::unique_ptr<MiniBird>>::iterator BlueBird::removeDestroyedMiniBird(std::list<std::unique_ptr<MiniBird>>::iterator it) {
    (*it)->getBody()->GetWorld()->DestroyBody((*it)->getBody());
    it = miniBirds_.erase(it);
    return it;
}


// Green bird power: The green bird can be controlled by the player after it has been launched
void GreenBird::usePower() {
    if (isPowerUsed_ || !isLaunched_) {
        return;
    }
    isPowerUsed_ = true;
}

void GreenBird::handleKeyPress(const sf::Keyboard::Key& code) {
    if (!isPowerUsed_) {
        return; // Power not yet activated
    }
    float controlForce = 20.0f;
    b2Vec2 force(0.0f, 0.0f);
    auto maxSpeed = 5.0f; // Maximum speed of the bird
    // Check for the key pressed and modify the force vector accordingly
    switch (code) {
        case sf::Keyboard::W:
            force.y = controlForce;
            break;
        case sf::Keyboard::S:
            force.y = -controlForce;
            break;
        case sf::Keyboard::A:
            force.x = -controlForce;
            break;
        case sf::Keyboard::D:
            force.x = controlForce;
            break;
    }

    // Apply the force to the bird's body at its center of mass to modify direction
    body_->ApplyForceToCenter(force, true);

    // Limit the speed of the bird
    b2Vec2 velocity = body_->GetLinearVelocity();
    float currentSpeed = velocity.Length();

    // Clamp the speed to the maximum value
    if (currentSpeed > maxSpeed) {
        velocity *= (maxSpeed / currentSpeed); // Normalize and scale down the velocity
        body_->SetLinearVelocity(velocity);
    }
}

void Bird::handleCollision(Object* objectB) {
    if (objectB->getType() == Object::Type::MiniBird) {
        return; // Bird gets no damage from MiniBirds
    }
    float damage = objectB->getBody()->GetLinearVelocity().LengthSquared();
    switch (objectB->getType()) {
        case Object::Type::Pig:
            damage = damage * 0.5;
            break;
        case Object::Type::Wall:
            damage = damage * 1.5f;
            break;
        case Object::Type::Ground:
            if (prevY_ > 0.95f) {
                damage = body_->GetLinearVelocity().Length() * 0.1f;
            } else {
                damage = 0;
            }
            break;
        default:
            break;
    }
    
    if (damage <= 0.1f) {
        return;
    }
    health_ -= damage * this->getDamageMultiplier();
    if (health_ <= 0) {
        isDestroyed_ = true;
    }
}

bool Bird::getIsPowerUsed() const {
    return isPowerUsed_;
}

int Bird::getDamageMultiplier() const {
    return 1;
}

int GreenBird::getDamageMultiplier() const {
    if (isPowerUsed_) {
        return 10;
    } else {
        return 1;
    }
}

char MiniBird::getTypeAsChar() const {
    return 'M';
}

bool MiniBird::shouldRemove() const {
    return isDestroyed() || isOutOfBounds();
}

void MiniBird::handleCollision(Object* objectB) {
    if (objectB->getType() == Object::Type::MiniBird
        || objectB->getType() == Object::Type::Bird
        || objectB->getType() == Object::Type::Ground
    ) {
        return; // MiniBirds don't damage each other and don't get damaged by other birds
    }
    float damage = objectB->getBody()->GetLinearVelocity().LengthSquared();
    switch (objectB->getType()) {
        case Object::Type::Pig:
            damage = damage * 2.f;
            break;
        case Object::Type::Wall:
            damage = damage * 2.f;
            break;
        default:
            break;
    }
    
    if (damage <= 0.1f) {
        return;
    }
    health_ -= damage * this->getDamageMultiplier();
    if (health_ <= 0) {
        isDestroyed_ = true;
    }
}

int MiniBird::getDamageMultiplier() const {
    return 2;
}