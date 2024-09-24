#ifndef SFML_DEBUG_DRAW
#define SFML_DEBUG_DRAW

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include "utils.hpp"

class SFMLDebugDraw : public b2Draw {
    public:
        SFMLDebugDraw(sf::RenderWindow& window) : window(window) {}

        void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override {
            sf::ConvexShape polygon(vertexCount);
            for (int i = 0; i < vertexCount; ++i) {
                sf::Vector2f pos = Box2DToSFML(vertices[i]);
                polygon.setPoint(i, pos);
            }
            polygon.setOutlineThickness(1.0f);
            polygon.setFillColor(sf::Color::Transparent);
            polygon.setOutlineColor(SFMLColor(color));
            window.draw(polygon);
        }

        void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override {
            sf::ConvexShape polygon(vertexCount);
            for (int i = 0; i < vertexCount; ++i) {
                sf::Vector2f transformedVec = Box2DToSFML(vertices[i]);
                polygon.setPoint(i, transformedVec);
            }
            polygon.setFillColor(SFMLColor(color));
            window.draw(polygon);
        }

        void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override {
            sf::CircleShape circle(radius);
            circle.setOrigin(radius / 2.0f, radius /2.0f);
            circle.setPosition(center.x, center.y);//Box2DToSFML(center));
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineThickness(1.0f);
            circle.setOutlineColor(SFMLColor(color));
            window.draw(circle);
        }

        void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override {
            sf::CircleShape circle(radius);
            circle.setOrigin(radius, radius);
            circle.setPosition(Box2DToSFML(center));
            circle.setFillColor(SFMLColor(color));
            // Optionally draw the axis line
            window.draw(circle);
        }

        void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override {
            sf::VertexArray lines(sf::Lines, 2);
            lines[0].position = Box2DToSFML(p1);
            lines[1].position = Box2DToSFML(p2);
            lines[0].color = lines[1].color = SFMLColor(color);
            window.draw(lines);
        }

        void DrawTransform(const b2Transform& xf) override {
            float lineLength = 0.4;
            // Axis lines
            b2Vec2 p1 = xf.p, p2;
            // Red (x-axis)
            p2 = p1 + lineLength * xf.q.GetXAxis();
            DrawSegment(p1, p2, b2Color(1,0,0));
            // Green (y-axis)
            p2 = p1 + lineLength * xf.q.GetYAxis();
            DrawSegment(p1, p2, b2Color(0,1,0));
        }

        void mDrawPoint(const b2Vec2& p, float size, const b2Color& color) {
            sf::CircleShape point(size);
            point.setFillColor(SFMLColor(color));
            point.setPosition(Box2DToSFML(p));
            window.draw(point);
        }

        void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override {
            mDrawPoint(p, size, color);
        }

    private:
        sf::RenderWindow& window;

        sf::Vector2f Box2DToSFML(const b2Vec2& vec) {
            return utils::B2ToSfCoords(vec);
        }

        sf::Color SFMLColor(const b2Color& color) {
            // Convert Box2D color to SFML color
            return sf::Color(color.r * 255, color.g * 255, color.b * 255);
        }
};

#endif // SFML_DEUB_DRAW