#include "cannon.hpp"
#include "utils.hpp"
#include <iomanip>
#include <sstream>

Cannon::Cannon(std::string barrel_texture_file, std::string wheels_texture_file, std::string font_file) {
    if (!utils::loadFromFile(barrel_texture_, barrel_texture_file)) {
        throw std::runtime_error("Failed to load texture file: " + barrel_texture_file);
    }
    if (!utils::loadFromFile(wheels_texture_, wheels_texture_file)) {
        throw std::runtime_error("Failed to load texture file: " + wheels_texture_file);
    }
    if (!utils::loadFromFile(font_, font_file)) {
        throw std::runtime_error("Failed to load font");
    }
    barrel_sprite_.setTexture(barrel_texture_);
    int width = barrel_sprite_.getTextureRect().width;
    int height = barrel_sprite_.getTextureRect().height;
    barrel_sprite_.setScale(2.f * SCALE / width, 2.f * SCALE / height);
    barrel_sprite_.setOrigin(width / 2.f, height / 2.f);
    barrel_sprite_.setPosition(utils::B2ToSfCoords(BIRD_INITIAL_POSITION));

    wheels_sprite_.setTexture(wheels_texture_);
    width = wheels_sprite_.getTextureRect().width;
    height = wheels_sprite_.getTextureRect().height;
    wheels_sprite_.setScale(2.f * SCALE / width, 2.f * SCALE / height);
    wheels_sprite_.setOrigin(width / 2.f, height / 2.f);
    wheels_sprite_.setPosition(utils::B2ToSfCoords(b2Vec2(BIRD_INITIAL_POSITION.x, BIRD_INITIAL_POSITION.y - 0.2f)));

    power_text_.setFont(font_);
    power_text_.setString("Power: 0 %");   
    power_text_.setFillColor(sf::Color::Black);
    power_text_.setCharacterSize(30);
    power_text_.setPosition(10, 10);
    
}

void Cannon::draw(sf::RenderWindow &window) const {
    window.draw(power_text_);
    window.draw(barrel_sprite_);
    window.draw(wheels_sprite_);
}

void Cannon::setAngle(float angle) {
    barrel_sprite_.setRotation(angle);
}

void Cannon::setPower(float duration) {
    power_ = std::min(4 * duration, 4.f);
    auto power_ratio= power_  * 100 / 4;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << power_ratio;
    std::string power_ratio_str = oss.str();
    power_text_.setString("Power: " + power_ratio_str + " %");
}

void Cannon::startLaunch() {
    pressClock_.restart();
    isLaunching_ = true;
}

void Cannon::launchBird(Bird* bird) {
    isLaunching_ = false;
    if (bird == nullptr) {
        return;
    }
    float direction = -barrel_sprite_.getRotation();
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

