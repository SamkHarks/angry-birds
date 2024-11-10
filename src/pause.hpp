#ifndef PAUSE_HPP
#define PAUSE_HPP

#include "menu.hpp"

class Pause : public Menu {
    public:
        Pause();
        void draw(sf::RenderWindow& window) const override;
        void handleResize(const sf::RenderWindow&) override;
        void handleResize() override;
        void updatePosition(const sf::RenderWindow& window);
    private:
        const std::vector<std::string>& getButtonNames() const override;
        void updateTitle(const sf::RenderWindow& window);
};

#endif // PAUSE_HPP