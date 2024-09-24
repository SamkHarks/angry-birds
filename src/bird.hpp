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
        Bird(b2Body *body, std::string texture_file, Type bird_type, float radius);
        virtual char getTypeAsChar() const override = 0;

        Type getBirdType() const;

        bool isLaunched() const;

        void setLaunched(bool launched);

        bool isMoving() const override;

        private:
            Type bird_type_;
            bool is_launched_ = false;
            sf::Clock pressClock_;
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