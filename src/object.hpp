#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>

/**
 * @brief Object class, the base class for all objects in the game, Bird, Pig, Ground, Wall and MiniBird
 */
class Object {
    public:
        enum class Type {
            Bird,
            Pig,
            Ground,
            Wall,
            MiniBird  // A smaller projectile of the original bird
        };
        Object(b2Body *body, Type type, std::string texture_file, float health = 0, bool isDestrucable = false);
        virtual ~Object() = default;
        virtual void draw(sf::RenderWindow &window) const;
        b2Body* getBody();
        const b2Body* getBody() const;
        sf::Sprite& getSprite();
        const sf::Sprite& getSprite() const;
        virtual char getTypeAsChar() const = 0;
        Type getType() const;
        virtual void handleCollision(Object* objectB);
        bool isDestroyed() const;
        virtual void update();
        virtual bool isMoving() const;
        virtual int getDestructionScore() const;
        bool isOutOfBounds() const;
        virtual bool shouldRemove() const;
        virtual int getDamageMultiplier() const { return 1; }
    protected:
        sf::Sprite sprite_;
        b2Body *body_;
        float health_;
        bool isDestrucable_;
        bool isDestroyed_ = false;
        float prevY_ = 0;
    private:
        
        Type type_;
       
};

#endif // OBJECT_HPP