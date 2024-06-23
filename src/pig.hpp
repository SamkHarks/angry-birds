#ifndef PIG_HPP
#define PIG_HPP

#include "object.hpp"

class Pig : public Object {
    public:
        Pig(b2Body *body);
        virtual char getTypeAsChar() const override;
};

#endif //PIG_HPP