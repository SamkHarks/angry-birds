#include "cannon.hpp"
#include <iomanip>
#include <sstream>

Cannon::Cannon() {
    ResourceManager& resourceManager = ResourceManager::getInstance();
    cannon_.init();

    powerText_.setFont(resourceManager.getFont("/assets/fonts/BerkshireSwash-Regular.ttf"));
    powerText_.setString("Power: 0 %");   
    powerText_.setFillColor(sf::Color::White);
    powerText_.setCharacterSize(40);
    powerText_.setPosition(20, 10);
    powerText_.setOutlineColor(sf::Color::Black);
    powerText_.setOutlineThickness(2);

    launchSound_.setBuffer(resourceManager.getSoundBuffer("/assets/sounds/cannon_fire.ogg"));
    launchSound_.setVolume(50);

}

void Cannon::handleResize() {
    cannon_.handleResize();
}

void Cannon::draw(sf::RenderWindow &window) const {
    window.draw(powerText_);
    cannon_.draw(window);
}

void Cannon::setAngle(float angle) {
    cannon_.barrelSprite.setRotation(angle);
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
    eventDispatcher_->dispatch(Event(Event::Type::BirdLaunching));
}

void Cannon::launchBird(Bird* bird) {
    isLaunching_ = false;
    if (bird == nullptr || bird->isLaunched()) {
        return;
    }
    launchSound_.play();
    // Activate bird's body in b2World
    bird->getBody()->SetEnabled(true);
    float direction = -cannon_.barrelSprite.getRotation();
    float x = cos(utils::DegreesToRadians(direction)) * power_;
    float y = sin(utils::DegreesToRadians(direction)) * power_;
    bird->getBody()->ApplyLinearImpulseToCenter(b2Vec2(x, y), true);
    bird->setLaunched(true);
    eventDispatcher_->dispatch(Event(Event::Type::BirdLaunched));
}

void Cannon::update() {
    if (isLaunching_) {
        float duration = pressClock_.getElapsedTime().asSeconds();
        setPower(duration);
        eventDispatcher_->dispatch(Event(Event::Type::BirdLaunching));
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

void Cannon::updateTextPosition(const sf::RenderWindow& window) {
    sf::Vector2i scorePosition(20, 10);
    powerText_.setPosition(window.mapPixelToCoords(scorePosition));
}

int Cannon::getTextWidth() const {
    return powerText_.getGlobalBounds().width;
}

void Cannon::handleMouseMove(const sf::Vector2f& mousePosition) {
    sf::Vector2f canonCenter = utils::B2ToSfCoords(BIRD_INITIAL_POSITION);
    sf::Vector2f difference = mousePosition - canonCenter;
    float direction = utils::getDirection(difference);
    setAngle(direction);
}

void Cannon::setEventDispatcher(EventDispatcher* eventDispatcher) {
    eventDispatcher_ = eventDispatcher;
}
