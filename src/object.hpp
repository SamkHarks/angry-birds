#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>

class Object {
    public:
        enum class Type {
            Bird,
            Pig,
            Ground,
            Wall,
        };
        Object(b2Body *body, Type type, std::string texture_file);
        virtual ~Object() = default;
        b2Body* getBody();
        const b2Body* getBody() const;
        sf::Sprite& getSprite();
        const sf::Sprite& getSprite() const;
        virtual char getTypeAsChar() const = 0;
        Type getType() const;

    protected:
        sf::Texture texture_;
        sf::Sprite sprite_;
        b2Body *body_;
    private:
        
        Type type_;
       
};

#endif // OBJECT_HPP