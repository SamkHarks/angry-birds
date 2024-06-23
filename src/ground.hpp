#ifndef GROUND_HPP
#define GROUND_HPP

#include "object.hpp"

class Ground : public Object {
    public:
        Ground(b2Body *body);
        char getTypeAsChar() const override;
};

#endif // GROUND_HPP