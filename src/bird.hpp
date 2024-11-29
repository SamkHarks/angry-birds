#ifndef BIRD_HPP
#define BIRD_HPP

#include "object.hpp"

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
        virtual void handleCollision(Object* objectB) override;
        virtual void handleKeyPress(const sf::Keyboard::Key& code) {};
        virtual int getDamageMultiplier() const override;
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
            bool isLaunched_ = false;
            bool isPaused_ = false;
            bool isPowerUsed_ = false;
            float pausedTime_ = 0;
            sf::Clock pressClock_;
            sf::Clock pauseClock_;
};

class RedBird : public Bird {
    public:
        RedBird(b2Body *body, float radius);
        virtual char getTypeAsChar() const override;
        virtual void usePower() override;
};

class BlueBird : public Bird {
    public:
        BlueBird(b2Body *body, float radius);
        virtual char getTypeAsChar() const override;
        virtual void usePower() override;
};

class GreenBird : public Bird {
    public:
        GreenBird(b2Body *body, float radius);
        virtual char getTypeAsChar() const override;
        virtual void usePower() override;
        void handleKeyPress(const sf::Keyboard::Key& code) override;
        int getDamageMultiplier() const override;
};



#endif // BIRD_HPP