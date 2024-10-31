#ifndef WALL_HPP
#define WALL_HPP

#include "object.hpp"

class Wall : public Object {
    public:
        Wall(b2Body *body, float hx, float hy);
        virtual char getTypeAsChar() const override;
        bool shouldRemove() const override;
};

#endif // WALL_HPP