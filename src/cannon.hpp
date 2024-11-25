#ifndef CANNON_HPP
#define CANNON_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "bird.hpp"
#include "utils.hpp"
#include "resource_manager.hpp"

struct CannonSprites {
    void init() {
        barrelSprite.setTexture(ResourceManager::getInstance().getTexture("/assets/images/cannon_barrel.png"));
        int width = barrelSprite.getTextureRect().width;
        int height = barrelSprite.getTextureRect().height;
        barrelSprite.setScale(2.f * SCALE / width, 2.f * SCALE / height);
        barrelSprite.setOrigin(width / 2.f, height / 2.f);
        barrelSprite.setPosition(utils::B2ToSfCoords(BIRD_INITIAL_POSITION));

        wheelsSprite.setTexture(ResourceManager::getInstance().getTexture("/assets/images/cannon_wheel.png"));
        width = wheelsSprite.getTextureRect().width;
        height = wheelsSprite.getTextureRect().height;
        wheelsSprite.setScale(2.f * SCALE / width, 2.f * SCALE / height);
        wheelsSprite.setOrigin(width / 2.f, height / 2.f);
        wheelsSprite.setPosition(utils::B2ToSfCoords(b2Vec2(BIRD_INITIAL_POSITION.x, BIRD_INITIAL_POSITION.y - 0.2f)));
    }

    void draw(sf::RenderWindow &window) const {
        window.draw(barrelSprite);
        window.draw(wheelsSprite);
    }

    void handleResize() {
        barrelSprite.setPosition(utils::B2ToSfCoords(BIRD_INITIAL_POSITION));
        wheelsSprite.setPosition(utils::B2ToSfCoords(b2Vec2(BIRD_INITIAL_POSITION.x, BIRD_INITIAL_POSITION.y - 0.2f)));
    }

    private:
        sf::Sprite barrelSprite;
        sf::Sprite wheelsSprite;
        friend class Cannon;
};

class Cannon {
    public:
        Cannon();
        void setAngle(float angle);
        void draw(sf::RenderWindow &window) const;
        void setPower(float duration);
        void launchBird(Bird* bird);
        void startLaunch();
        void update();
        bool isLaunching() const;
        void reset();
        void updateTextPosition(const sf::RenderWindow& window);
        int getTextWidth() const;
        void handleResize();
        void handleMouseMove(const sf::Vector2f& mousePosition);
    private:
        CannonSprites cannon_;
        sf::Text powerText_;
        sf::Sound launchSound_;
        sf::Clock pressClock_;
        float power_ = 0;
        bool isLaunching_ = false;
};

#endif // CANNON_HPP