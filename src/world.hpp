#ifndef WORLD_HPP
#define WORLD_HPP

#include <box2d/box2d.h>
#include <list>
#include "bird.hpp"
#include "ground.hpp"
#include "pig.hpp"
#include "wall.hpp"
#include "cannon.hpp"
#include "level_loader.hpp"
#include "resource_manager.hpp"
#include "score.hpp"
#include "user_loader.hpp"

struct SfObject {
    sf::Sprite sprite;
    sf::Text text;
    char type;
    int count;
};

class World {
    public:
        World();
        ~World();
        void loadLevel(const std::string& filename);
        void clearLevel();
        void resetLevel();
        void saveHighScore(int score);
        void addObject(Object *object);
        void step();
        void draw(sf::RenderWindow &window) const;
        Bird *GetBird();
        const Bird* GetBird() const;
        b2World* getWorld();
        std::list<Object *> getObjects();
        void removeObject(Object *object);
        void removeBird();
        void resetBird();
        int getRemainingBirdCount() const;
        int getRemainingPigCount() const;
        Cannon* getCannon();
        void updateScore(int score);
        int getStars() const;
        Score& getScore();
        const std::string& getLevelName() const;
        void setPlayer(const Player& player);
        const Player& getPlayer() const;
        void updateRemainingCountPositions(const sf::RenderWindow& window);
        void updateRemainingCounts(char type);
        int getAliveBirdCount() const;
        int getLevelIndex() const;
        bool updatePlayer();
        bool isSettled () const;
    private:
        b2World *world_;
        b2Vec2 gravity_;
        std::list<Object *> objects_;
        std::list<Bird *> birds_;
        std::string levelName_;
        int levelIndex_ = 0;
        Cannon *cannon_;
        sf::RectangleShape background_;
        sf::Texture background_image_;
        int totalPigCount_ = 0;
        int totalBirdCount_ = 0;
        std::string fileName_;
        Score scoreManager_;
        Player player_ = {"", std::vector<int>(), std::vector<int>()};
        void drawRemainingCounts(sf::RenderWindow &window) const;
        std::list<SfObject> sfObjects_;
        LevelLoader levelLoader_;
        friend class LevelLoader;
};

#endif // WORLD_HPP