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
        Type getBirdType() const;
        bool isLaunched() const;
        void setLaunched(bool launched);
        void setIsPaused(bool paused);
        bool getIsPaused() const;
        bool shouldRemove() const override;
        float getActiveTime() const;
        private:
            Type birdType_;
            bool isLaunched_ = false;
            bool isPaused_ = false;
            float pausedTime_ = 0;
            sf::Clock pressClock_;
            sf::Clock pauseClock_;
};

class RedBird : public Bird {
    public:
        RedBird(b2Body *body, float radius);
        virtual char getTypeAsChar() const override;
};

class BlueBird : public Bird {
    public:
        BlueBird(b2Body *body, float radius);
        virtual char getTypeAsChar() const override;
};

class GreenBird : public Bird {
    public:
        GreenBird(b2Body *body, float radius);
        virtual char getTypeAsChar() const override;
};



#endif // BIRD_HPP