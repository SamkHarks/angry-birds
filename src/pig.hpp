#ifndef PIG_HPP
#define PIG_HPP

#include "object.hpp"

const int PIG_DESTRUCTION_SCORE = 1000;

class Pig : public Object {
    public:
        Pig(b2Body *body, float radius);
        virtual char getTypeAsChar() const override;
        virtual int getDestructionScore() const override;
        virtual bool shouldRemove() const override;
};

#endif //PIG_HPP