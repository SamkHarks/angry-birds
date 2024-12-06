#ifndef BIRD_HPP
#define BIRD_HPP

#include "object.hpp"
#include <list>
#include <memory>

/**
* @brief MiniBird class, a smaller projectile of the original bird class. Only used by BlueBird when it uses its power
*/
class MiniBird : public Object {
    public:
        MiniBird(b2Body *body, float radius);
        char getTypeAsChar() const override;
        bool shouldRemove() const override;
        void handleCollision(Object* objectB) override;
        int getDamageMultiplier() const override;
};

/**
 * @brief Bird class, the base class for all birds in the game, RedBird, BlueBird and GreenBird
 */
class Bird : public Object {
    public:
        enum class Type {
            Red,
            Blue,
            Green,
        };
        Bird(b2Body *body, std::string textureFile, Type birdType, float radius);
        virtual char getTypeAsChar() const override = 0;
        virtual void usePower() = 0;
        virtual void draw(sf::RenderWindow &window) const override;
        virtual void update() override;
        virtual void handleCollision(Object* objectB) override;
        virtual void handleKeyPress(const sf::Keyboard::Key& code) {};
        virtual int getDamageMultiplier() const override;
        ~Bird() override = default;
        Type getBirdType() const;
        bool isLaunched() const;
        void setLaunched(bool launched);
        void setIsPaused(bool paused);
        bool getIsPaused() const;
        bool shouldRemove() const override;
        float getActiveTime() const;
        bool getIsPowerUsed() const;
        protected:
            Type birdType_;
            float radius_;
            bool isLaunched_ = false;
            bool isPaused_ = false;
            bool isPowerUsed_ = false;
            float pausedTime_ = 0;
            sf::Clock pressClock_;
            sf::Clock pauseClock_;
};

/**
 * @brief RedBird class inherits from Bird class, bird that can apply a downward force when its power is used
 */
class RedBird : public Bird {
    public:
        RedBird(b2Body *body, float radius);
        char getTypeAsChar() const override;
        void usePower() override;
};

/**
 * @brief BlueBird class inherits from Bird class, bird that can split into 2 MiniBirds when its power is used
 */
class BlueBird : public Bird {
    public:
        BlueBird(b2Body *body, float radius);
        void draw(sf::RenderWindow &window) const override;
        char getTypeAsChar() const override;
        void update() override;
        void usePower() override;
        void destroyMiniBirds();
        std::list<std::unique_ptr<MiniBird>>::iterator removeDestroyedMiniBird(std::list<std::unique_ptr<MiniBird>>::iterator it);
    private:
        void createMiniBird(b2Vec2 position);
        std::list<std::unique_ptr<MiniBird>> miniBirds_;
};

/**
 * @brief GreenBird class inherits from Bird class, bird that can be controlled by the player after the power has been activated
 */
class GreenBird : public Bird {
    public:
        GreenBird(b2Body *body, float radius);
        char getTypeAsChar() const override;
        void usePower() override;
        void handleKeyPress(const sf::Keyboard::Key& code) override;
        int getDamageMultiplier() const override;
};



#endif // BIRD_HPP