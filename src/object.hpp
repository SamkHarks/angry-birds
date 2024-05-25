#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <box2d/box2d.h>


class Object {
    public:
        enum class Type {
            Bird,
            Pig,
            Ground,
            Wall,
        };
        Object(b2Body *body);
        virtual ~Object() = default;
        b2Body* getBody();

    private:
        b2Body *body_;
       
};

#endif // OBJECT_HPP