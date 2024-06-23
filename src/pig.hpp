#ifndef PIG_HPP
#define PIG_HPP

#include "object.hpp"

class Pig : public Object {
    public:
        Pig(b2Body *body, float radius);
        virtual char getTypeAsChar() const override;
};

#endif //PIG_HPP