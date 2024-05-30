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
        Bird(b2Body *body, std::string texture_file, Type bird_type);
        virtual char getTypeAsChar() const = 0;

        Type getBirdType() const;

        private:
            Type bird_type_;
};

class RedBird : public Bird {
    public:
        RedBird(b2Body *body);
        virtual char getTypeAsChar() const override;
};



#endif // BIRD_HPP