#ifndef WALL_HPP
#define WALL_HPP

#include "object.hpp"

class Wall : public Object {
    public:
        Wall(b2Body *body);
        virtual char getTypeAsChar() const override;
};

#endif // WALL_HPP