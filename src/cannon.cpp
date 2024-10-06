#include "cannon.hpp"
#include "utils.hpp"
#include <iomanip>
#include <sstream>
#include "resource_manager.hpp"

Cannon::Cannon() {
    if (!utils::loadFromFile(barrelTexture_, "/assets/images/cannon_barrel.png")) {
        throw std::runtime_error("Failed to load texture file: /assets/images/cannon_barrel.png");
    }
    if (!utils::loadFromFile(wheelsTexture_, "/assets/images/cannon_wheel.png")) {
        throw std::runtime_error("Failed to load texture file: /assets/images/cannon_wheel.png");
    }
    font_ = ResourceManager::getInstance().getFont("/assets/fonts/BerkshireSwash-Regular.ttf");
    barrelSprite_.setTexture(barrelTexture_);
    int width = barrelSprite_.getTextureRect().width;
    int height = barrelSprite_.getTextureRect().height;
    barrelSprite_.setScale(2.f * SCALE / width, 2.f * SCALE / height);
    barrelSprite_.setOrigin(width / 2.f, height / 2.f);
    barrelSprite_.setPosition(utils::B2ToSfCoords(BIRD_INITIAL_POSITION));

    wheelsSprite_.setTexture(wheelsTexture_);
    width = wheelsSprite_.getTextureRect().width;
    height = wheelsSprite_.getTextureRect().height;
    wheelsSprite_.setScale(2.f * SCALE / width, 2.f * SCALE / height);
    wheelsSprite_.setOrigin(width / 2.f, height / 2.f);
    wheelsSprite_.setPosition(utils::B2ToSfCoords(b2Vec2(BIRD_INITIAL_POSITION.x, BIRD_INITIAL_POSITION.y - 0.2f)));

    powerText_.setFont(font_);
    powerText_.setString("Power: 0 %");   
    powerText_.setFillColor(sf::Color::White);
    powerText_.setCharacterSize(40);
    powerText_.setPosition(20, 10);
    powerText_.setOutlineColor(sf::Color::Black);
    powerText_.setOutlineThickness(2);
}

void Cannon::draw(sf::RenderWindow &window) const {
    window.draw(powerText_);
    window.draw(barrelSprite_);
    window.draw(wheelsSprite_);
}

void Cannon::setAngle(float angle) {
    barrelSprite_.setRotation(angle);
}

void Cannon::setPower(float duration) {
    power_ = std::min(4 * duration, 4.f);
    auto power_ratio= power_  * 100 / 4;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << power_ratio;
    std::string power_ratio_str = oss.str();
    powerText_.setString("Power: " + power_ratio_str + " %");
}

void Cannon::startLaunch() {
    pressClock_.restart();
    isLaunching_ = true;
}

void Cannon::launchBird(Bird* bird) {
    isLaunching_ = false;
    if (bird == nullptr || bird->isLaunched()) {
        return;
    }
    // Activate bird's body in b2World
    bird->getBody()->SetEnabled(true);
    float direction = -barrelSprite_.getRotation();
    float x = cos(utils::DegreesToRadians(direction)) * power_;
    float y = sin(utils::DegreesToRadians(direction)) * power_;
    bird->getBody()->ApplyLinearImpulseToCenter(b2Vec2(x, y), true);
    bird->setLaunched(true);
}

void Cannon::update() {
    if (isLaunching_) {
        float duration = pressClock_.getElapsedTime().asSeconds();
        setPower(duration);
    }
}

bool Cannon::isLaunching() const {
    return isLaunching_;
}

void Cannon::reset() {
    power_ = 0;
    isLaunching_ = false;
    powerText_.setString("Power: 0 %");
}

void Cannon::updateTextPosition(sf::RenderWindow& window) {
    sf::Vector2i scorePosition(20, 10);
    powerText_.setPosition(window.mapPixelToCoords(scorePosition));
}

